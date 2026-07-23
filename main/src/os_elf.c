#include <elf.h>
#include <stdio.h>
#include <string.h>

#include "os_psram.h"

void elf_file_seek(FILE *app, int offset)
{
  fseek(app,offset,SEEK_SET);
}

int elf_file_read(FILE *app, void *buffer, int len)
{
  return fread(buffer,1,len,app);
}

void elf_get_strtab(FILE *app, int e_shoff, int e_shstrndx, char *strtab)
{
  Elf32_Shdr shdr;
  elf_file_seek(app, e_shoff + sizeof(shdr) * e_shstrndx);
  elf_file_read(app, &shdr, sizeof(shdr));
  elf_file_seek(app, shdr.sh_offset);
  elf_file_read(app, strtab, shdr.sh_size);
}

uint32_t elf_place_in_ram(FILE *app, int offset, int filesize, int ramsize)
{
  if(filesize >= ramsize)
  {
    ramsize = filesize;
  }
  ramsize += 3;
  ramsize &= ~3;

  uint8_t *mem = os_psram_code_malloc(ramsize);
  while(((uint32_t)mem)&3)
  {
    mem++;
  }
  elf_file_seek(app,offset);
  elf_file_read(app,mem,filesize);
  return (uint32_t)mem;
}

uint32_t elf_load_sections(FILE *app, uint32_t e_entry, int e_shoff, int e_shnum, char *strtab)
{
  int i;
  Elf32_Shdr shdr;
  int offset;

  uint32_t *global_table;
  int global_table_size = 0;

  uint32_t text = 0;
  uint32_t text_len = 0;
  uint32_t text_vaddr = 0;

  uint32_t rodata = 0;
  uint32_t rodata_len = 0;
  uint32_t rodata_vaddr = 0;

  uint32_t data = 0;
  uint32_t data_len = 0;
  uint32_t data_vaddr = 0;

  uint32_t bss = 0;
  uint32_t bss_len = 0;
  uint32_t bss_vaddr = 0;

  offset = e_shoff;
  for(i=0;i<e_shnum;i++)
  {
    elf_file_seek(app, offset);
    offset += sizeof(shdr);
    elf_file_read(app,&shdr,sizeof(shdr));
    if(!strcmp(&strtab[shdr.sh_name],".text"))
    {
      text = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      text_len = shdr.sh_size;
      text_vaddr = shdr.sh_addr;
      //relocate entry to new address
    }
    if(!strcmp(&strtab[shdr.sh_name],".rodata"))
    {
      rodata = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      rodata_len = shdr.sh_size;
      rodata_vaddr = shdr.sh_addr;
    }
    if(!strcmp(&strtab[shdr.sh_name],".data"))
    {
      data = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      data_len = shdr.sh_size;
      data_vaddr = shdr.sh_addr;
    }
    if(!strcmp(&strtab[shdr.sh_name],".bss"))
    {
      bss = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      bss_len = shdr.sh_size;
      bss_vaddr = shdr.sh_addr;
    }
    if(!strcmp(&strtab[shdr.sh_name],".xt.lit"))
    {
      uint32_t got_meta[2];
      int j;
      elf_file_seek(app, shdr.sh_offset);
      global_table_size = 0;
      for(j=0;j<shdr.sh_size;j+=8)
      {
        elf_file_read(app,got_meta,8);
        global_table_size += got_meta[1];
      }
      global_table_size /= 4;
      printf("global table size %d\n",global_table_size);
    }
  }
  if(bss > 0)
  {
    for(i=0;i<bss_len;i++)
    {
      ((uint8_t *)bss)[i] = 0;
    }
  }
  printf("Text %08lX => %08lX\n",text_vaddr, text);
  printf("size %ld\n",text_len);
  printf("Rodata %08lX => %08lX\n",rodata_vaddr, rodata);
  printf("size %ld end %08lX\n",rodata_len,rodata_vaddr + rodata_len);
  printf("Data %08lX => %08lX\n",data_vaddr, data);
  printf("size %ld end %08lX\n",data_len,data_vaddr + data_len);
  printf("Bss %08lX => %08lX\n",bss_vaddr,bss);
  printf("size %ld\n",bss_len);
  global_table = (uint32_t *)text;
  for(i=0;i<global_table_size;i++)
  {
    if(global_table[i] >= text_vaddr && global_table[i] <= text_vaddr + text_len)
    {
      printf("function pointer %08lX",global_table[i]);
      global_table[i] -= text_vaddr;
      global_table[i] += text;
      global_table[i] += 0x6000000; //hardware thing
    }
    if(global_table[i] >= rodata_vaddr && global_table[i] <= rodata_vaddr + rodata_len)
    {
      printf("rodata pointer %08lX",global_table[i]);
      global_table[i] -= rodata_vaddr;
      global_table[i] += rodata;
    }
    if(global_table[i] >= data_vaddr && global_table[i] <= data_vaddr + data_len)
    {
      printf("data pointer %08lX",global_table[i]);
      global_table[i] -= data_vaddr;
      global_table[i] += data;
    }
    if(global_table[i] >= bss_vaddr && global_table[i] <= bss_vaddr + bss_len)
    {
      printf("bss pointer %08lX",global_table[i]);
      global_table[i] -= bss_vaddr;
      global_table[i] += bss;
    }
    printf(" => %08lX\n",global_table[i]);
  }
  e_entry -= text_vaddr;
  e_entry += text;
  e_entry += 0x6000000; //hardware thing
  printf("new entry %08lX\n",e_entry);
  return e_entry;
}

uint32_t elf_load(const char *filename)
{
  char strtab[256];
  uint32_t entry;
  Elf32_Ehdr e32_hdr;
  size_t rv;
  FILE *app = fopen(filename,"r");

  if(!app)
  {
    printf("App not found\n");
    return 0;
  }
  rv = elf_file_read(app,&e32_hdr,sizeof(e32_hdr));
  if(rv != sizeof(e32_hdr))
  {
    printf("rv %d\n",rv);
    printf("App not valid\n");
    fclose(app);
    return 0;
  }
  elf_get_strtab(app,e32_hdr.e_shoff,e32_hdr.e_shstrndx,strtab);
  entry = elf_load_sections(app,e32_hdr.e_entry,e32_hdr.e_shoff,e32_hdr.e_shnum,strtab);
  //entry = elf_load_sections(app,e32_hdr.e_phoff,e32_hdr.e_phnum,e32_hdr.e_entry);
  fclose(app);
  return entry;
}