#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "os_psram.h"

#define ELF_DEBUG

#ifdef ELF_DEBUG
#define ELF_LOG(...) printf(__VA_ARGS__)
#else
#define ELF_LOG(...)
#endif

//WORKAROUND
#ifndef R_XTENSA_NONE
#define R_XTENSA_NONE           0
#define R_XTENSA_32             1
#endif

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

  uint32_t *global_table = 0;
  int global_table_size = 0;

  uint32_t *global_table_2 = 0;
  int global_table_size_2 = 0;

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
    else if(!strcmp(&strtab[shdr.sh_name],".rodata"))
    {
      rodata = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      rodata_len = shdr.sh_size;
      rodata_vaddr = shdr.sh_addr;
    }
    else if(!strcmp(&strtab[shdr.sh_name],".data"))
    {
      data = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      data_len = shdr.sh_size;
      data_vaddr = shdr.sh_addr;
    }
    else if(!strcmp(&strtab[shdr.sh_name],".bss"))
    {
      bss = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      bss_len = shdr.sh_size;
      bss_vaddr = shdr.sh_addr;
    }
    else if(!strcmp(&strtab[shdr.sh_name],".xt.lit"))
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
      ELF_LOG("global table size %d\n",global_table_size);
    }
    else if(!strcmp(&strtab[shdr.sh_name],".rela.text"))
    {
      ELF_LOG("relocation %s\n",&strtab[shdr.sh_name]);
      Elf32_Rela reloc;
      int j;
      elf_file_seek(app, shdr.sh_offset);
      for(j=0;j<shdr.sh_size;j+=sizeof(reloc))
      {
        elf_file_read(app,&reloc,sizeof(reloc));
        ELF_LOG("DUMP: %08lX %08lX %08lX\n",reloc.r_offset,reloc.r_info,reloc.r_addend);
      }
    }
    else if(!strcmp(&strtab[shdr.sh_name],".rela.data"))
    {
      ELF_LOG("relocation %s\n",&strtab[shdr.sh_name]);
      Elf32_Rela reloc;
      int j;
      elf_file_seek(app, shdr.sh_offset);
      for(j=0;j<shdr.sh_size;j+=sizeof(reloc))
      {
        elf_file_read(app,&reloc,sizeof(reloc));
        if(ELF32_R_TYPE(reloc.r_info) == R_XTENSA_32)
        {
          ELF_LOG("relocating data pointer\n");
          reloc.r_offset += data;
          reloc.r_offset -= data_vaddr;
          global_table_2 = realloc(global_table_2,++global_table_size_2);
          global_table_2[global_table_size_2-1] = reloc.r_offset;
        }
        
        ELF_LOG("DUMP: %08lX %08lX %08lX\n",reloc.r_offset,reloc.r_info,reloc.r_addend);
      }
    }
    else if(!strncmp(&strtab[shdr.sh_name],".rel",4))
    {
      /*
      ELF_LOG("relocation %s\n",&strtab[shdr.sh_name]);
      uint32_t reloc_data[3];
      int j;
      elf_file_seek(app, shdr.sh_offset);
      for(j=0;j<shdr.sh_size;j+=12)
      {
        elf_file_read(app,reloc_data,12);
        ELF_LOG("DUMP: %08lX %08lX %08lX\n",reloc_data[0],reloc_data[1],reloc_data[2]);
      }
      */
    }
    else
    {
      ELF_LOG("segment not used %s\n",&strtab[shdr.sh_name]);
    }
  }
  if(bss > 0)
  {
    for(i=0;i<bss_len;i++)
    {
      ((uint8_t *)bss)[i] = 0;
    }
  }
  ELF_LOG("TEXT from (%08lX - %08lX)\n", text_vaddr, text_vaddr + text_len);
  ELF_LOG("TEXT to   (%08lX - %08lX)\n", text, text + text_len);
  ELF_LOG("RODA from (%08lX - %08lX)\n", rodata_vaddr, rodata_vaddr + rodata_len);
  ELF_LOG("RODA to   (%08lX - %08lX)\n", rodata, rodata + rodata_len);
  ELF_LOG("DATA from (%08lX - %08lX)\n", data_vaddr, data_vaddr + data_len);
  ELF_LOG("DATA to   (%08lX - %08lX)\n", data, data + data_len);
  ELF_LOG("BSS  from (%08lX - %08lX)\n", bss_vaddr, bss_vaddr + bss_len);
  ELF_LOG("BSS  to   (%08lX - %08lX)\n", bss, bss + bss_len);
  //ELF_LOG("Text %08lX => %08lX\n",text_vaddr, text);
  //ELF_LOG("size %ld\n",text_len);
  //ELF_LOG("Rodata %08lX => %08lX\n",rodata_vaddr, rodata);
  //ELF_LOG("size %ld end %08lX\n",rodata_len,rodata_vaddr + rodata_len);
  //ELF_LOG("Data %08lX => %08lX\n",data_vaddr, data);
  //ELF_LOG("size %ld end %08lX\n",data_len,data_vaddr + data_len);
  //ELF_LOG("Bss %08lX => %08lX\n",bss_vaddr,bss);
  //ELF_LOG("size %ld\n",bss_len);
  global_table = (uint32_t *)text;
  for(i=0;i<global_table_size;i++)
  {
    if(global_table[i] >= text_vaddr && global_table[i] < text_vaddr + text_len)
    {
      ELF_LOG("function pointer %08lX",global_table[i]);
      global_table[i] -= text_vaddr;
      global_table[i] += text;
      global_table[i] += 0x6000000; //hardware thing
    }
    else if(global_table[i] >= rodata_vaddr && global_table[i] < rodata_vaddr + rodata_len)
    {
      ELF_LOG("rodata pointer %08lX",global_table[i]);
      global_table[i] -= rodata_vaddr;
      global_table[i] += rodata;
    }
    else if(global_table[i] >= data_vaddr && global_table[i] < data_vaddr + data_len)
    {
      ELF_LOG("data pointer %08lX",global_table[i]);
      global_table[i] -= data_vaddr;
      global_table[i] += data;
    }
    else if(global_table[i] >= bss_vaddr && global_table[i] < bss_vaddr + bss_len)
    {
      ELF_LOG("bss pointer %08lX",global_table[i]);
      global_table[i] -= bss_vaddr;
      global_table[i] += bss;
    }
    ELF_LOG(" => %08lX\n",global_table[i]);
  }
  for(i=0;i<global_table_size_2;i++)
  {
    uint32_t *temp_ptr;
    ELF_LOG("global table 2 %08lX\n",global_table_2[i]);
    temp_ptr = (uint32_t *)global_table_2[i];
    ELF_LOG("ptr to relocate %08lX\n",temp_ptr[0]);
    if(temp_ptr[0] >= text_vaddr && temp_ptr[0] < text_vaddr + text_len)
    {
      temp_ptr[0] -= text_vaddr;
      temp_ptr[0] += text;
      temp_ptr[0] += 0x6000000; //hardware thing
    }
    else if(temp_ptr[0] >= rodata_vaddr && temp_ptr[0] < rodata_vaddr + rodata_len)
    {
      temp_ptr[0] -= rodata_vaddr;
      temp_ptr[0] += rodata;
    }
    else if(temp_ptr[0] >= data_vaddr && temp_ptr[0] < data_vaddr + data_len)
    {
      temp_ptr[0] -= data_vaddr;
      temp_ptr[0] += data;
    }
    else if(temp_ptr[0] >= bss_vaddr && temp_ptr[0] < bss_vaddr + bss_len)
    {
      temp_ptr[0] -= bss_vaddr;
      temp_ptr[0] += bss;
    }
    ELF_LOG("ptr to relocate %08lX\n",temp_ptr[0]);
  }
  if(global_table_2)
  {
    free(global_table_2);
  }
  e_entry -= text_vaddr;
  e_entry += text;
  e_entry += 0x6000000; //hardware thing
  ELF_LOG("new entry %08lX\n",e_entry);
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
    //ELF_LOG("App not found\n");
    return 0;
  }
  rv = elf_file_read(app,&e32_hdr,sizeof(e32_hdr));
  if(rv != sizeof(e32_hdr))
  {
    ELF_LOG("rv %d\n",rv);
    ELF_LOG("App not valid\n");
    fclose(app);
    return 0;
  }
  elf_get_strtab(app,e32_hdr.e_shoff,e32_hdr.e_shstrndx,strtab);
  entry = elf_load_sections(app,e32_hdr.e_entry,e32_hdr.e_shoff,e32_hdr.e_shnum,strtab);
  //entry = elf_load_sections(app,e32_hdr.e_phoff,e32_hdr.e_phnum,e32_hdr.e_entry);
  fclose(app);
  return entry;
}