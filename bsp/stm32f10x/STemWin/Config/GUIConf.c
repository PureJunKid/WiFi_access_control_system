#include "GUI.h"
#include <rtthread.h>


#define USE_EXRAM  0//ʹ���ڲ�RAM
//����EMWIN�ڴ��С
#define GUI_NUMBYTES  (150*1024)
#define GUI_BLOCKSIZE 256  //���С


//GUI_X_Config
//��ʼ����ʱ�����,��������emwin��ʹ�õ��ڴ�
void GUI_X_Config(void) 
{
	U32 *aMemory = rt_malloc(GUI_NUMBYTES); //���ڲ�RAM�з���GUI_NUMBYTES�ֽڵ��ڴ�
//	rt_memset(aMemory, 0, GUI_NUMBYTES);
	GUI_ALLOC_AssignMemory((U32 *)aMemory, GUI_NUMBYTES); //Ϊ�洢����ϵͳ����һ���洢��
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //���ô洢���ƽ���ߴ�,����Խ��,���õĴ洢������Խ��
	GUI_SetDefaultFont(GUI_FONT_6X8); //����Ĭ������
}
