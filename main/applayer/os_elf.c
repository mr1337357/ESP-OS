#include <elf.h>

void elf_get_strtab(File &app, int e_shoff, int e_shstrndx, char *strtab)
{
  Elf32_Shdr shdr;
  app.seek(e_shoff + (sizeof(shdr) * e_shstrndx));
  app.read((uint8_t *)&shdr,sizeof(shdr));
  app.seek(shdr.sh_offset);
  app.read((uint8_t *)strtab,shdr.sh_size);
}

uint32_t elf_place_in_ram(File &app, int offset, int filesize, int ramsize)
{
  uint8_t *mem = (uint8_t *)psram_malloc(ramsize);
  app.seek(offset);
  app.read(mem,filesize);
  return (uint32_t)mem;
}

uint32_t elf_load_sections(File &app, uint32_t e_entry, int e_shoff, int e_shnum, char *strtab)
{
  int i;
  Elf32_Shdr shdr;
  int offset;

  uint32_t *global_table;
  int global_table_size;

  uint32_t text = 0;
  uint32_t text_len = 0;
  uint32_t text_vaddr;

  uint32_t rodata = 0;
  uint32_t rodata_len = 0;
  uint32_t rodata_vaddr;

  uint32_t data = 0;
  uint32_t data_len = 0;
  uint32_t data_vaddr;

  uint32_t bss = 0;
  uint32_t bss_len = 0;
  uint32_t bss_vaddr;

  offset = e_shoff;
  for(i=0;i<e_shnum;i++)
  {
    app.seek(offset);
    offset += sizeof(shdr);
    app.read((uint8_t *)&shdr,sizeof(shdr));
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
      app.seek(shdr.sh_offset);
      global_table_size = 0;
      for(j=0;j<shdr.sh_size;j+=8)
      {
        app.read((uint8_t *)got_meta,8);
        global_table_size += got_meta[1];
      }
      global_table_size /= 4;
      Serial.printf("global table size %d\n",global_table_size);
    }
  }
  if(bss > 0)
  {
    for(i=0;i<bss_len;i++)
    {
      ((uint8_t *)bss)[i] = 0;
    }
  }
  Serial.printf("Text %08X => %08X\n",text_vaddr, text);
  Serial.printf("size %d\n",text_len);
  Serial.printf("Rodata %08X => %08X\n",rodata_vaddr, rodata);
  Serial.printf("size %d end %08X\n",rodata_len,rodata_vaddr + rodata_len);
  Serial.printf("Data %08X => %08X\n",data_vaddr, data);
  Serial.printf("size %d end %08X\n",data_len,data_vaddr + data_len);
  Serial.printf("Bss %08X => %08X\n",bss_vaddr,bss);
  Serial.printf("size %d\n",bss_len);
  global_table = (uint32_t *)text;
  for(i=0;i<global_table_size;i++)
  {
    if(global_table[i] >= text_vaddr && global_table[i] <= text_vaddr + text_len)
    {
      Serial.printf("function pointer %08X",global_table[i]);
      global_table[i] -= text_vaddr;
      global_table[i] += text;
      global_table[i] += 0x6000000; //hardware thing
    }
    if(global_table[i] >= rodata_vaddr && global_table[i] <= rodata_vaddr + rodata_len)
    {
      Serial.printf("rodata pointer %08X",global_table[i]);
      global_table[i] -= rodata_vaddr;
      global_table[i] += rodata;
    }
    if(global_table[i] >= data_vaddr && global_table[i] <= data_vaddr + data_len)
    {
      Serial.printf("data pointer %08X",global_table[i]);
      global_table[i] -= data_vaddr;
      global_table[i] += data;
    }
    if(global_table[i] >= bss_vaddr && global_table[i] <= bss_vaddr + bss_len)
    {
      Serial.printf("bss pointer %08X",global_table[i]);
      global_table[i] -= bss_vaddr;
      global_table[i] += bss;
    }
    Serial.printf(" => %08X\n",global_table[i]);
  }
  e_entry -= text_vaddr;
  e_entry += text;
  e_entry += 0x6000000; //hardware thing
  Serial.printf("new entry %08X\n",e_entry);
  return e_entry;
}

uint32_t loadElf(fs::FS &fs, const char *filename)
{
  char strtab[256];
  uint32_t entry;
  Elf32_Ehdr e32_hdr;
  File app = fs.open(filename);

  if(!app)
  {
    Serial.printf("App not found\n");
    return 0;
  }
  if(app.read((uint8_t *)&e32_hdr,sizeof(e32_hdr))!=sizeof(e32_hdr))
  {
    Serial.printf("App not valid\n");
    goto FAILURE;
  }
  elf_get_strtab(app,e32_hdr.e_shoff,e32_hdr.e_shstrndx,strtab);
  entry = elf_load_sections(app,e32_hdr.e_entry,e32_hdr.e_shoff,e32_hdr.e_shnum,strtab);
  //entry = elf_load_sections(app,e32_hdr.e_phoff,e32_hdr.e_phnum,e32_hdr.e_entry);
  app.close();
  return entry;
FAILURE:
  app.close();
  return 0;
}