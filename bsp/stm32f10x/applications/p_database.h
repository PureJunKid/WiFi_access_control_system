/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: p_database.h
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 08 ��
**
** ��        ��: �û���Ϣ���ݿ�

** ��־:
2016.09.08  �������ļ�
*********************************************************************************************************/

#ifndef _P_DATABASE_H_
#define _P_DATABASE_H_

#include <rtthread.h>
#include <stdint.h>

/* ��ӡ������Ϣ */
#if 0
#define printDebugInfo
#endif

/* ����û����� */
#define MAX_USER_NUM        200

/* header��С */
#define HEADER_SIZE         8

#define GET_USER            0    //��ȡ�û�
#define SET_USER            1    //�����û�

#define ADD_ONE_USER        0    //����û�
#define DEL_ONE_USER        1    //ɾ���û�
#define GET_ONE_USER        2    //��ȡ�û���Ϣ

/** \brief �û���Ϣ�ṹ ��1058�ֽ� */
__packed typedef struct user_info
{
    uint16_t user_id;         /**< \brief 0-1         �û��� */
    uint32_t card_id;         /**< \brief 2-5         ���� */
    uint8_t  effective;       /**< \brief 6           ����״̬ 0:ʧЧ 1:��Ч */
    uint8_t  student_id[16];  /**< \brief 7-22        ѧ�� 11������(ǰ��0) */
    uint8_t  name[16];        /**< \brief 23-38       ���� �ұ߲�0x00 */
    uint8_t  authority[16];   /**< \brief 39-54       Ȩ�� */
    uint8_t  is_time_limit;   /**< \brief 55          �Ƿ���ʱ������ 0:������ 1:������ */
    uint16_t year;            /**< \brief 56-57       ��(��Ч�ڣ�����֮���û�ʧЧ) */
    uint8_t  month;           /**< \brief 58          �� */
    uint8_t  day;             /**< \brief 59          �� */
    uint8_t  hour;            /**< \brief 60          ʱ */
    uint8_t  minutes;         /**< \brief 61          �� */
    uint8_t  second;          /**< \brief 62          �� */
    uint16_t finger_index[5]; /**< \brief 63-72       ָ������ֵ�ֱ���ָ��ģ���е��û��� */
    uint8_t  finger[5][193];  /**< \brief 73-1037     ָ������ֵ */
    uint32_t crc;             /**< \brief 1038-1041   ����Ա֮ǰ���г�Ա���ֽڼ����CRCֵ */
    uint8_t  state[16];       /**< \brief 1042-1057   ����״̬ */
} user_info_t;

/** \brief �������û��Žṹ */
__packed typedef struct card_id_user_id_link
{
    uint16_t user_id;        /**< \brief �û��� */
    uint32_t card_id;        /**< \brief ���� */
} card_id_user_id_link_t;

/** \brief ָ�ƺ����û��Žṹ */
__packed typedef struct finger_user_id_id_link
{
    uint16_t user_id;        /**< \brief �û��� */
    uint16_t finger_id;      /**< \brief ָ�ƺ� */
} finger_id_user_id_link_t;

extern const char *user_info_database_path;

/* ֻ�����û��š����ŵĽṹ������,�ɿ�������֮��ʹ��2�ַ����� */
extern card_id_user_id_link_t g_card_id_user_id_list[MAX_USER_NUM];

/**
 * \brief ����-�û�����������������������ıȽϺ���(�ݼ�)
 *
 * \param[in] p_a ��Աa
 * \param[in] p_b ��Աb
 *
 * \return a����b����С��0��a����b����0��aС��b���ش���0
 */
int card_compar (const void *p_a, const void *p_b);

/**
 * \brief ��ȡ�û�����������û���
 *
 * \param[out] p_user_num    �û�����
 * \param[out] p_max_user_id ����û���
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t user_num_get (uint16_t *p_user_num, uint16_t *p_max_user_id);

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
int8_t user_crc_get (uint32_t crc[], uint16_t *p_num_get, uint16_t start, uint16_t num);

extern int8_t get_set_user_num(uint16_t *user_num, uint8_t cmd);//��ȡ/�������ݿ��е���Ա��Ϣ
extern int8_t get_set_user_num_max(uint16_t *user_num_max, uint8_t cmd);//��ȡ/�������ݿ��е�����û���
extern int8_t add_del_get_one_user(user_info_t *one_user_info, uint8_t cmd);//���/ɾ��/��ȡ���ݿ��е�һ���û�(��Ҫ����one_user_info->user_id����)
int8_t get_set_state(uint8_t *state, uint16_t user_id, uint8_t cmd);
extern int8_t card_array_init(card_id_user_id_link_t card_array[]);//��ʼ�����Žṹ����

#endif
