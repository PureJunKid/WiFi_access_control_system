/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: p_database.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 08 ��
**
** ��        ��: �û���Ϣ���ݿ�

** ��־:
2016.09.08  �������ļ�
*********************************************************************************************************/
#include "p_database.h"

#include <rtthread.h>
#include <finsh.h>
#include <dfs.h>
#include <dfs_posix.h>
#include <stdlib.h>
#include "global.h"

/** \brief ���õ�����Ϣ��ӡ���� */
#ifdef printDebugInfo
#define p_printf(fmt,args...) rt_kprintf("p_printf: "); rt_kprintf(fmt, ##args)
#else
#define p_printf(fmt,args...)
#endif

/** \brief �û���Ϣ���ݿ�·�� */
const char *user_info_database_path = "/member.bin";

/** \brief ֻ�������š��û��ŵĽṹ������,�ɿ�������֮��ʹ��2�ַ����� */
card_id_user_id_link_t g_card_id_user_id_list[MAX_USER_NUM];

/** \brief ֻ����ָ�ƺš��û��ŵĽṹ������,��ָ�ƺ�����֮��ʹ��2�ַ����� */
finger_id_user_id_link_t g_finger_id_user_id_list[MAX_USER_NUM];

/**
 * \brief ����-�û�����������������������ıȽϺ���(�ݼ�)
 *
 * \param[in] p_a ��Աa
 * \param[in] p_b ��Աb
 *
 * \return a����b����С��0��a����b����0��aС��b���ش���0
 */
int card_compar (const void *p_a, const void *p_b)
{
    if (((card_id_user_id_link_t *)p_a)->card_id > ((card_id_user_id_link_t *)p_b)->card_id) {
        return -1;
    } else 
    if (((card_id_user_id_link_t *)p_a)->card_id < ((card_id_user_id_link_t *)p_b)->card_id) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * \brief ��ȡ�û�����������û���
 *
 * \param[out] p_user_num    �û�����
 * \param[out] p_max_user_id ����û���
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t user_num_get (uint16_t *p_user_num, uint16_t *p_max_user_id)
{
    int fd = 0;
    struct stat file_info;
    uint16_t user_id = 0;
    uint16_t user_num = 0;
    uint16_t max_user_num = 0;
    uint32_t i = 0;
    uint32_t addr = 0;
    
    /* ��ȡ�ļ���Ϣ */
    if (stat(user_info_database_path, &file_info) < 0) {
        p_printf("stat %s failed\r\n", user_info_database_path);
        return -1;
    }
    
    /* ����Ա��Ϣ���ݿ� */
    fd = open(user_info_database_path, O_RDWR | O_CREAT, 0);
    if (fd < 0) {
        p_printf("open %s failed\r\n", user_info_database_path);
        close(fd);
        return -1;
    }
    
    /* ���������û���������û��� */
    for (i = 0; i < (file_info.st_size - HEADER_SIZE) / sizeof(user_info_t); i++) {
        addr = (sizeof(user_info_t) * i) + HEADER_SIZE;

        /* �ƶ��ļ�ָ�뵽ָ��λ�� */
        if (lseek(fd, addr, SEEK_SET) == -1) {
            p_printf("lseek addr %d %s failed\r\n", addr, user_info_database_path);
            goto failed;
        } 

        /* ��ȡ�û��� */
        if (read(fd, &user_id, 2) != 2)
        {
            p_printf("read %s failed\r\n", user_info_database_path);
            goto failed;
        }
        
        if (user_id != 0) {
            user_num++;
            if (user_id > max_user_num) {
                max_user_num = user_id;
            }
        }
    }
    
    if (RT_NULL != p_user_num) {
        *p_user_num = user_num;
    }
    if (RT_NULL != p_max_user_id) {
        *p_max_user_id = max_user_num;
    }

    close(fd);
    return 0;

failed:
    close(fd);
    return -1;
}
FINSH_FUNCTION_EXPORT_ALIAS(user_num_get, user_num_get, user_num_get)

/*******************************************************************************
* ������     : add_del_get_one_user
* ����       : ���/ɾ��/��ȡ���ݿ��е�һ���û�(��Ҫ����one_user_info->user_id����)
* ����         : - one_user_info: ��Ա��Ϣ�ṹ�� - cmd: 0: ��� 1: ɾ�� 2: ��ȡ
* ���         : None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
int8_t add_del_get_one_user (user_info_t *one_user_info, uint8_t cmd)
{
    int fd;

    /* ����Ա��Ϣ���ݿ� */
    fd = open(user_info_database_path, O_RDWR|O_CREAT, 0);
    if (fd < 0)
    {
        p_printf("open %s failed\r\n", user_info_database_path);
        close(fd);
        return -1;
    }
    else
    {
        /* �ƶ��ļ�ָ�뵽ָ��λ�� */
        if (lseek(fd, (sizeof(user_info_t)*(one_user_info->user_id-1)) + HEADER_SIZE, SEEK_SET) == -1)
        {
            p_printf("lseek %s failed\r\n", user_info_database_path);
            close(fd);
            return -1;
        }
        else
        {
            switch(cmd)
            {
                
                /* ����û� */
                case 0:
                {
                    /* д���û���Ϣ */
                    if (write(fd, one_user_info, sizeof(user_info_t)) != sizeof(user_info_t))
                    {
                        p_printf("write %s failed\r\n", user_info_database_path);
                        close(fd);
                        return -1;
                    }
                    else
                    {
                        close(fd);
                        p_printf("add user %d success\r\n", one_user_info->user_id);            
                    }
                }    break;
                
                /* ɾ���û� */
                case 1:
                {
                    /* д���û���Ϣ */
                    rt_memset(one_user_info, 0,sizeof(user_info_t));//���û���Ϣ����
                    if (write(fd, one_user_info, sizeof(user_info_t)) != sizeof(user_info_t))
                    {
                        p_printf("write %s failed\r\n", user_info_database_path);
                        close(fd);
                        return -1;
                    }
                    else
                    {
                        close(fd);
                        p_printf("del user %d success\r\n", one_user_info->user_id);            
                    }
                }    break;
                
                /* ��ȡ�û���Ϣ */
                case 2:
                {
                    /* ��ȡ�û���Ϣ */
                    if (read(fd, one_user_info, sizeof(user_info_t)) != sizeof(user_info_t))
                    {
                        p_printf("read %s failed\r\n", user_info_database_path);
                        close(fd);
                        return -1;
                    }
                    else
                    {
                        close(fd);
                        p_printf("get user %d success\r\n", one_user_info->user_id);            
                    }
                }    break;
                
                default :
                {
                    close(fd);
                    p_printf("add_del_get one user cmd unknown\r\n");                    
                    return -1;
                }
            }
        }
    }
    
    return 0;
}

/**
 * \brief ��ȡָ�������û�CRC
 *
 * \param[out] crc       ��ȡ�����û�crc
 * \param[out] p_num_get ��ȡ�����û�crc����
 * \param[in]  start     ��ʼ�û���
 * \param[in]  num       ����ȡ������
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t user_crc_get (uint32_t crc[], uint16_t *p_num_get, uint16_t start, uint16_t num)
{
    int fd;
    
    uint16_t user_num     = 0;
    uint16_t user_id_max  = 0;
    
    uint32_t i         = 0;
    uint32_t addr      = 0;
    uint32_t get_count = 0;
    
    /* ��ȡ�û�����������û��� */
    if (user_num_get(&user_num, &user_id_max) != 0) {
        p_printf("user_num_get failed\r\n");
        return -1;
    }
    
    if (start > user_id_max) {
        p_printf("start > user_id_max\r\n");
        if (RT_NULL != p_num_get) {
            *p_num_get = 0;
        }
        return 0;
    } else if (start == 0) {
        p_printf("start == 0\r\n");
        return -1;
    } else if (start + num > user_id_max) {
        num = user_id_max - start + 1;
    }
        
    /* ����Ա��Ϣ���ݿ� */
    fd = open(user_info_database_path, O_RDWR|O_CREAT, 0);
    if (fd < 0)
    {
        p_printf("open %s failed\r\n", user_info_database_path);
        return -1;
    }

    /* ��ȡ�û�crc */
    for (i = start; i < start + num; i++) {
        
        /* �����ļ�ƫ�� */
        addr = (sizeof(user_info_t) * (i - 1)) + OFFSET(user_info,crc) + HEADER_SIZE;
        
        /* �ƶ��ļ�ָ�뵽ָ��λ�� */
        if (lseek(fd, addr, SEEK_SET) == -1) {
            p_printf("lseek %s failed\r\n", user_info_database_path);
            goto failed;
        }

        /* ��ȡ�û���Ϣ */
        if (read(fd, &crc[get_count], 4) != 4) {
            p_printf("read %s failed\r\n", user_info_database_path);
            goto failed;
        }
        get_count++;
    }
    
    if (RT_NULL != p_num_get) {
        *p_num_get = get_count;
    }
            
    close(fd);
    return 0;
    
failed:
    close(fd);
    return -1;
}

void crc_test(uint16_t start, uint16_t num)
{
    uint32_t crc[20];
    uint16_t num_get;
//    uint16_t start = 1;
//    uint16_t num = 100;
    
    if (user_crc_get(crc, &num_get, start, num) != 0)
    {
        rt_kprintf("user_crc_get failed\r\n");
    } else {
        rt_kprintf("num_get %d\r\n", num_get);
        for (int i = 0; i < num_get; i++) {
            rt_kprintf("%08X ", crc[i]);
        }
        rt_kprintf("\r\n");
    }
}
FINSH_FUNCTION_EXPORT_ALIAS(crc_test, crc_test, crc_test)

/*******************************************************************************
* ������     : get_set_state
* ����       : ��ȡ/����ָ���û��Ľ���״̬
* ����         : - state: ״̬ - user_id: �û��� - cmd: 0: ��ȡ״̬ 1: ����״̬
* ���         : None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
int8_t get_set_state(uint8_t *state, uint16_t user_id, uint8_t cmd)
{
    int fd;

    /* ����Ա��Ϣ���ݿ� */
    fd = open(user_info_database_path, O_RDWR|O_CREAT, 0);
    if (fd < 0)
    {
        p_printf("open %s failed\r\n", user_info_database_path);
        close(fd);
        return -1;
    }    
    else
    {
        /* �ƶ��ļ�ָ�뵽ָ��λ�� */
        if (lseek(fd, (sizeof(user_info_t)*(user_id-1)) + OFFSET(user_info,state) + HEADER_SIZE, SEEK_SET) == -1)
        {
            p_printf("lseek %s failed\r\n", user_info_database_path);
            close(fd);
            return -1;
        }
        else
        {
            switch(cmd)
            {
                /* ��ȡ����״̬ */
                case 0:
                {
                    /* ��ȡ����״̬ */
                    if (read(fd, state, MEMBER_SIZE(user_info,state)) != MEMBER_SIZE(user_info,state))
                    {
                        p_printf("read %s failed\r\n", user_info_database_path);
                        close(fd);
                        return -1;
                    }
                    else
                    {
                        close(fd);
                        p_printf("get state %d success\r\n", user_id);            
                    }
                }    break;
                /* ���ý���״̬ */
                case 1:
                {
                    /* д�����״̬ */
                    if (write(fd, state, MEMBER_SIZE(user_info,state)) != MEMBER_SIZE(user_info,state))
                    {
                        p_printf("write %s failed\r\n", user_info_database_path);
                        close(fd);
                        return -1;
                    }
                    else
                    {
                        close(fd);
                        p_printf("set state %d success\r\n", user_id);            
                    }
                }    break;
                default :
                {
                    close(fd);
                    p_printf("get set state cmd unknown\r\n");                    
                    return -1;
                }
            }
        }
    }
    
    return 0;
}

/**
 * \brief ��ʼ������-�û�������
 *
 * \param[out] card_array ����-�û�������
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t card_array_init(card_id_user_id_link_t card_array[])
{
    int fd;
    int i;
    uint16_t user_num;
    uint16_t max_user_id;
    uint16_t effective_num = 0;
    uint32_t addr = 0;
    
    /* ��ȡ�û�����������û��� */
    if(user_num_get(&user_num, &max_user_id) == -1) {
        p_printf("get user num max failed\r\n");
        return -1;
    }
    
    p_printf("user_num: %d  max_user_num: %d\r\n", user_num, max_user_id);

    /* ����Ա��Ϣ���ݿ� */
    fd = open(user_info_database_path, O_RDWR|O_CREAT, 0);
    if (fd < 0)
    {
        p_printf("open %s failed\r\n", user_info_database_path);
        goto failed;
    }

    for (i = 1; i <= max_user_id; i++) {
        
        addr = (sizeof(user_info_t)*(i-1)) + HEADER_SIZE;
        p_printf("lseek addr %d\r\n", addr);

        /* �ƶ��ļ�ָ�뵽ָ��λ�� */
        if (lseek(fd, addr, SEEK_SET) == -1) {
            p_printf("lseek addr %d %s failed\r\n", addr, user_info_database_path);
            goto failed;
        } 
        
        /* ��ȡ�û���Ϣ */
        if (read(fd, (void *)&card_array[effective_num], sizeof(card_id_user_id_link_t)) != sizeof(card_id_user_id_link_t)) {
            p_printf("read %s failed\r\n", user_info_database_path);
            goto failed;
        }
        
        if (card_array[effective_num].user_id != 0) {
            effective_num++;
            p_printf("%d: get user %d, %08X\r\n", i, card_array[effective_num - 1].user_id, card_array[effective_num - 1].card_id);            
        }

    }
    
    /* �ж��û������Ƿ���ȷ */
    if (effective_num != user_num) {
        p_printf("init card array failed! user_num is %d  effective_num is %d\r\n", user_num, effective_num);
        goto failed;
    }

    /* ʹ�ÿ��������㷨���������� */    
    qsort(card_array, MAX_USER_NUM, sizeof(card_id_user_id_link_t), card_compar);

    close(fd);
    return 0;
    
failed:
    close(fd);
    return -1;

}
