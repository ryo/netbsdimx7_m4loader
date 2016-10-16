#ifndef _MEM_H_
#define _MEM_H_

void memopen(void);
void memclose(void);
uint32_t memread4(uint32_t);
void memwrite4(uint32_t, uint32_t);
int32_t memread(uint32_t, void *buf, uint32_t);
int32_t memwrite(uint32_t, const void *buf, uint32_t);

#endif /* _MEM_H_ */
