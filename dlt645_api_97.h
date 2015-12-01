/****************************************************
    File           dlt645_api_97.h
    Brief         dlt645 ��Լ 1997�� ��������ͷ�ļ�
    Detail        ʵ�ֶ�dlt645 1997���Լ���Ͷ���
                �ṩ֡���ݵķ�װ���������api    
    Author         liming
    Date           2012/10/20
    Version        1.0
    Functions
    CopyRight    ����������ѧԺͨ�����õ缼���ֹ�˾
    History    

****************************************************/
#ifndef _DLT645_API_1997_H_
#define _DLT645_API_1997_H_

#ifndef _TYPE_DEF_LM_
#define _TYPE_DEF_LM_
#ifndef __RTL_H__
#define __RTL_H__
typedef char    BOOL;
#endif
#define TRUE    (1)
#define FALSE   (0)
typedef unsigned char UCHAR;
typedef char    CHAR;
typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifndef _TYPE_DEF_
#define _TYPE_DEF_
typedef char            INT8;       //c
typedef short           INT16;      //s
typedef int             INT32;      //l
typedef unsigned char   UINT8;      //uc
typedef unsigned short  UINT16;     //us
typedef unsigned int    UINT32;     //ul

#ifdef MACHINE_64_BITS
typedef long             INT64;
typedef unsigned   long  UINT64;
#else
typedef long  long           INT64;
typedef unsigned long  long  UINT64;
#endif

typedef char*           STRING;     //str
typedef char            ISBOOL;     //b
typedef unsigned int    IPADDR;     //ip
typedef double          DOUBLE;
typedef void*  FRWK_HANDLE;

#endif
#endif



//#define D97_DEBUG_ON

#ifdef D97_DEBUG_ON
#define OUT_BUF_LEN 1024
#define D97_ON  0
#define D97_OFF 1

extern int g_d97_need_pack;
extern char g_d97_out_data[OUT_BUF_LEN];
void debug_switch_d97(int show);
#endif


/* �������� */
#define UNKNOWN_LEN        0xFFFFFFFF              /* δ֪���� */
#define D97_DATA_MAX_NR    8                       /* dlt645 1997 ����������ֽ������ԼID */
#define D97_DATA_MAX       (D97_DATA_MAX_NR + 2)   /* dlt645 1997 ����������ֽ��� */
#define D97_ADDR_LEN       12                      /* ��ַ�� */
#define D97_FRAME_LEN_MIN  12                        /* DLT645 97 ��С֡�ֽ��� */    

/* ��������ֵ */
#define YEAR_P_MAX            0xF
#define INTERVAL_M_MAX     10
#define DAY_Q_MAX          8
#define RATE_K_MAX         14           /* */
#define HOLIDAY_N_MAX      100          /* ���������� N���ֵ */
#define D97_NAME_LEN       64           /* ������ֳ��� (�ֽ���)*/

/* ������C�� ������ */
#define CTRL_CODE_RESERVE                    0x00   /*����*/
#define CTRL_CODE_READ_DATA                  0x01   /*��ȡ����*/
#define CTRL_CODE_READ_AFTERDATA            0x02   /*��ȡ��������*/
#define CTRL_CODE_READ_DATA_RE              0x03   /*�ض�����*/
#define CTRL_CODE_WRITE_DATA                0x04   /*д����*/
#define CTRL_CODE_SYNC_TIME                 0x08   /*�㲥Уʱ*/
#define CTRL_CODE_WRITE_ADDR                0x0A   /*д�豸��ַ*/
#define CTRL_CODE_MODIFY_BAUD                0x0C   /*����ͨ������*/
#define CTRL_CODE_MODIFY_PASSWORD           0x0F   /*��������*/
#define CTRL_CODE_MAXDEMAND_CLEAR           0x10   /*����������� 0*/


/* ȫ�ֱ��� */
extern unsigned char g_year_p;      /* ��ʱ���� P(1~F)*/
extern unsigned char g_interval_m;  /*��ʱ��(ÿ���л���)m (1~10)*/
extern unsigned char g_day_q;       /*��ʱ�α��� q (1~8)*/
extern unsigned char g_rate_k;      /* ������ k (1~14)*/
extern unsigned char g_holiday_n;   /* ���������� n*/


/* ���Ͷ��� */

/* ֡����ת����־ ���� �ӿڽṹ������֡����ת������ */
typedef enum
{
    E_D97_TRANS_U2F,    /* ������� user -> frame ���û��ӿ�����ת�� �� ֡����*/
    E_D97_TRANS_F2U,    /* ������� frame-> user  */    
}E_D97_TRANS_FLAG;


typedef struct
{
    BOOL bRateErr;  // ��������
    BOOL bDayErr;   // ��ʱ������
    BOOL bYearErr;  // ��ʱ������
    BOOL bPwdErr;   // ������δ��Ȩ
    BOOL bErrID;    // ���ݱ�ʶ��
    BOOL bErrData;  // �Ƿ�����
}S_D97_ERR;


/*  ��Լ���� */
typedef enum
{
    E_D97_RULER_UNKNOWN,                     // δ֪��Լ����    
    E_D97_RULER_STANDARD_MIN,               /* dlt645 97 ��׼��Լ������Сֵ */
    /* ���� */
    E_D97_RULER_STANDARD_POWER_MIN = E_D97_RULER_STANDARD_MIN,         /* ��ǩ��Լ����������ֵ��Ӧ��A.1*/
    E_D97_RULER_FORTH_HAVE_POWER_TOTAL = E_D97_RULER_STANDARD_MIN,   /* �����й��ܵ��� */
    E_D97_RULER_FORTH_HAVE_POWER_RATE,    /* �����й����ʵ���*/
    E_D97_RULER_FORTH_HAVE_POWER_BLOCK,      /* �����й��������ݿ� */

    E_D97_RULER_BACK_HAVE_POWER_TOTAL,   /* �����й��ܵ��� */
    E_D97_RULER_BACK_HAVE_POWER_RATE,    /* �����й����ʵ���*/
    E_D97_RULER_BACK_HAVE_POWER_BLOCK,   /* �����й��������ݿ� */

    E_D97_RULER_FORTH_NONE_POWER_TOTAL,   /* �����޹��ܵ��� */
    E_D97_RULER_FORTH_NONE_POWER_RATE,    /* �����޹����ʵ���*/
    E_D97_RULER_FORTH_NONE_POWER_BLOCK,      /* �����޹��������ݿ� */

    E_D97_RULER_BACK_NONE_POWER_TOTAL,   /* �����޹��ܵ��� */
    E_D97_RULER_BACK_NONE_POWER_RATE,    /* �����޹����ʵ���*/
    E_D97_RULER_BACK_NONE_POWER_BLOCK,   /* �����޹��������ݿ� */

    E_D97_RULER_QUAD_1_NONE_POWER_TOTAL,   /* 1 �����޹��ܵ��� */
    E_D97_RULER_QUAD_1_NONE_POWER_RATE,    /* 1 �����޹����ʵ���*/
    E_D97_RULER_QUAD_1_NONE_POWER_BLOCK,   /* 1 �����޹��������ݿ� */
    
    E_D97_RULER_QUAD_4_NONE_POWER_TOTAL,   /* 4 �����޹��ܵ��� */
    E_D97_RULER_QUAD_4_NONE_POWER_RATE,    /* 4 �����޹����ʵ���*/
    E_D97_RULER_QUAD_4_NONE_POWER_BLOCK,   /* 4 �����޹��������ݿ� */

    E_D97_RULER_QUAD_2_NONE_POWER_TOTAL,   /* 2 �����޹��ܵ��� */
    E_D97_RULER_QUAD_2_NONE_POWER_RATE,    /* 2 �����޹����ʵ���*/
    E_D97_RULER_QUAD_2_NONE_POWER_BLOCK,   /* 2 �����޹��������ݿ� */

    E_D97_RULER_QUAD_3_NONE_POWER_TOTAL,   /* 3 �����޹��ܵ��� */
    E_D97_RULER_QUAD_3_NONE_POWER_RATE,    /* 3 �����޹����ʵ���*/
    E_D97_RULER_QUAD_3_NONE_POWER_BLOCK,   /* 3 �����޹��������ݿ� */

    /* ��ǩ��Լ����������ֵ��Ӧ��A.1*/
    E_D97_RULER_STANDARD_POWER_MAX = E_D97_RULER_QUAD_3_NONE_POWER_BLOCK, 

    /* ������� */
    /* ��ǩ��Լ�������������Сֵ��Ӧ��A.2*/
    E_D97_RULER_STANDARD_DEMAND_MIN = E_D97_RULER_STANDARD_POWER_MAX + 1, 
     /* �����й���������� */
    E_D97_RULER_FORTH_HAVE_DEMAND_TOTAL = E_D97_RULER_STANDARD_DEMAND_MIN,  
    E_D97_RULER_FORTH_HAVE_DEMAND_RATE,    /* �����й������������*/
    E_D97_RULER_FORTH_HAVE_DEMAND_BLOCK,   /* �����й�����������ݿ� */

    E_D97_RULER_BACK_HAVE_DEMAND_TOTAL,   /* �����й���������� */
    E_D97_RULER_BACK_HAVE_DEMAND_RATE,    /* �����й������������*/
    E_D97_RULER_BACK_HAVE_DEMAND_BLOCK,   /* �����й�����������ݿ� */

    E_D97_RULER_FORTH_NONE_DEMAND_TOTAL,   /* �����޹���������� */
    E_D97_RULER_FORTH_NONE_DEMAND_RATE,    /* �����޹������������*/
    E_D97_RULER_FORTH_NONE_DEMAND_BLOCK,      /* �����޹�����������ݿ� */

    E_D97_RULER_BACK_NONE_DEMAND_TOTAL,   /* �����޹���������� */
    E_D97_RULER_BACK_NONE_DEMAND_RATE,    /* �����޹������������*/
    E_D97_RULER_BACK_NONE_DEMAND_BLOCK,   /* �����޹�����������ݿ� */

    E_D97_RULER_QUAD_1_NONE_DEMAND_TOTAL,   /* 1 �����޹���������� */
    E_D97_RULER_QUAD_1_NONE_DEMAND_RATE,    /* 1 �����޹������������*/
    E_D97_RULER_QUAD_1_NONE_DEMAND_BLOCK,   /* 1 �����޹�����������ݿ� */
    
    E_D97_RULER_QUAD_4_NONE_DEMAND_TOTAL,   /* 4 �����޹���������� */
    E_D97_RULER_QUAD_4_NONE_DEMAND_RATE,    /* 4 �����޹������������*/
    E_D97_RULER_QUAD_4_NONE_DEMAND_BLOCK,   /* 4 �����޹�����������ݿ� */

    E_D97_RULER_QUAD_2_NONE_DEMAND_TOTAL,   /* 2 �����޹���������� */
    E_D97_RULER_QUAD_2_NONE_DEMAND_RATE,    /* 2 �����޹������������*/
    E_D97_RULER_QUAD_2_NONE_DEMAND_BLOCK,   /* 2 �����޹�����������ݿ� */

    E_D97_RULER_QUAD_3_NONE_DEMAND_TOTAL,   /* 3 �����޹���������� */
    E_D97_RULER_QUAD_3_NONE_DEMAND_RATE,    /* 3 �����޹������������*/
    E_D97_RULER_QUAD_3_NONE_DEMAND_BLOCK,   /* 3 �����޹�����������ݿ� */

    /* ��ǩ��Լ��������������ֵ��Ӧ��A.2*/
    E_D97_RULER_STANDARD_DEMAND_MAX = E_D97_RULER_QUAD_3_NONE_DEMAND_BLOCK, 

    /* �����������ʱ�� */
    E_D97_RULER_FORTH_HAVE_DEMAND_TIME_TOTAL,   /* �����й��������������ʱ�� */
    E_D97_RULER_FORTH_HAVE_DEMAND_TIME_RATE,    /* �����й����������������ʱ��*/
    E_D97_RULER_FORTH_HAVE_DEMAND_TIME_BLOCK,    /* �����й������������ʱ�����ݿ� */

    E_D97_RULER_BACK_HAVE_DEMAND_TIME_TOTAL,   /* �����й��������������ʱ�� */
    E_D97_RULER_BACK_HAVE_DEMAND_TIME_RATE,    /* �����й����������������ʱ��*/
    E_D97_RULER_BACK_HAVE_DEMAND_TIME_BLOCK,   /* �����й������������ʱ�����ݿ� */

    E_D97_RULER_FORTH_NONE_DEMAND_TIME_TOTAL,   /* �����޹��������������ʱ�� */
    E_D97_RULER_FORTH_NONE_DEMAND_TIME_RATE,    /* �����޹����������������ʱ��*/
    E_D97_RULER_FORTH_NONE_DEMAND_TIME_BLOCK,    /* �����޹������������ʱ�����ݿ� */

    E_D97_RULER_BACK_NONE_DEMAND_TIME_TOTAL,       /* �����޹��������������ʱ�� */
    E_D97_RULER_BACK_NONE_DEMAND_TIME_RATE,        /* �����޹����������������ʱ��*/
    E_D97_RULER_BACK_NONE_DEMAND_TIME_BLOCK,       /* �����޹������������ʱ�����ݿ� */

    E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_TOTAL,   /* 1 �����޹��������������ʱ�� */
    E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_RATE,    /* 1 �����޹����������������ʱ��*/
    E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_BLOCK,   /* 1 �����޹������������ʱ�����ݿ� */

    E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_TOTAL,   /* 4 �����޹��������������ʱ�� */
    E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_RATE,    /* 4 �����޹����������������ʱ��*/
    E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_BLOCK,   /* 4 �����޹������������ʱ�����ݿ� */

    E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_TOTAL,   /* 2 �����޹��������������ʱ�� */
    E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_RATE,    /* 2 �����޹����������������ʱ��*/
    E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_BLOCK,   /* 2 �����޹������������ʱ�����ݿ� */

    E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_TOTAL,   /* 3 �����޹��������������ʱ�� */
    E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_RATE,    /* 3 �����޹����������������ʱ��*/
    E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_BLOCK,   /* 3 �����޹������������ʱ�����ݿ� */

    /* ��ǩ��Լ�������������ʱ�����ֵ��Ӧ��A.2*/
    E_D97_RULER_STANDARD_DEMAND_TIME_MAX = E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_BLOCK, 

    /* ��ǩ��Լ���͹̶������ֵ ��Ӧ��A1-A3*/
    E_D97_RULER_STANDARD_CONSTANT_MAX = E_D97_RULER_STANDARD_DEMAND_TIME_MAX, 

    /* �������� */
    
    /*����������Сֵ */
    E_D97_RULER_STANDARD_VAR_MIN = E_D97_RULER_STANDARD_CONSTANT_MAX + 1, 
    
    /* ���һ�α��ʱ�� */
    E_D97_RULER_RECENT_PROGRAME_TIME = E_D97_RULER_STANDARD_VAR_MIN,      
    E_D97_RULER_RECENT_CLEAR_DEMAND_TIME,        /* ���һ�����������0ʱ�� */
    E_D97_RULER_PROGRAME_TIMES,                     /* ��̴��� */
    E_D97_RULER_CLEAR_DEMAND_TIMES,                 /* ���������0���� */
    E_D97_RULER_CELL_WORK_TIMES,                 /* ��ع���ʱ�� */
    E_D97_RULER_PHASE_LOSS_TOTAL_TIMES,             /* �ܶ������ */
    E_D97_RULER_PHASE_A_LOSS_TIMES,                 /* A�������� */
    E_D97_RULER_PHASE_B_LOSS_TIMES,              /* B�������� */
    E_D97_RULER_PHASE_C_LOSS_TIMES,                 /* C�������� */
    E_D97_RULER_PHASE_LOSS_SPEND,                   /* �ܶ���ʱ���ۻ�ֵ */
    E_D97_RULER_PHASE_A_LOSS_SPEND,               /* A�����ʱ���ۻ�ֵ */
    E_D97_RULER_PHASE_B_LOSS_SPEND,               /* B�����ʱ���ۻ�ֵ */
    E_D97_RULER_PHASE_C_LOSS_SPEND,               /* C�����ʱ���ۻ�ֵ */

    E_D97_RULER_RECENT_PHASE_LOSS_START_TIME,      /* ���һ�ζ�����ʼʱ�� */
    E_D97_RULER_RECENT_PHASE_A_LOSS_START_TIME,     /* A�����һ�ζ�����ʼʱ�� */
    E_D97_RULER_RECENT_PHASE_B_LOSS_START_TIME,  /* B�����һ�ζ�����ʼʱ�� */
    E_D97_RULER_RECENT_PHASE_C_LOSS_START_TIME,     /* C�����һ�ζ�����ʼʱ�� */

    E_D97_RULER_RECENT_PHASE_LOSS_END_TIME,          /* ���һ�ζ������ʱ�� */
    E_D97_RULER_RECENT_PHASE_A_LOSS_END_TIME,       /* A�����һ�ζ������ʱ�� */
    E_D97_RULER_RECENT_PHASE_B_LOSS_END_TIME,    /* B�����һ�ζ������ʱ�� */
    E_D97_RULER_RECENT_PHASE_C_LOSS_END_TIME,    /* C�����һ�ζ������ʱ�� */
    E_D97_RULER_PHASE_A_VOLT,                     /* A���ѹ */
    E_D97_RULER_PHASE_B_VOLT,                     /* B���ѹ */
    E_D97_RULER_PHASE_C_VOLT,                     /* C���ѹ */
    E_D97_RULER_PHASE_A_ELEC,                     /* A����� */
    E_D97_RULER_PHASE_B_ELEC,                     /* B����� */
    E_D97_RULER_PHASE_C_ELEC,                     /* C����� */

    E_D97_RULER_INSTANT_HAVE_POWER_RATE,         /* ˲ʱ�й����� */
    E_D97_RULER_PHASE_A_HAVE_POWER_RATE,         /* A���й�����  */
    E_D97_RULER_PHASE_B_HAVE_POWER_RATE,         /* B���й�����  */
    E_D97_RULER_PHASE_C_HAVE_POWER_RATE,         /* C���й�����  */

    E_D97_RULER_FORTH_HAVE_POWER_RATE_LIMIT,     /* �����й��������� */
    E_D97_RULER_BACK_HAVE_POWER_RATE_LIMIT,         /* �����й��������� */

    E_D97_RULER_INSTANT_NONE_POWER_RATE,         /* ˲ʱ�޹����� */
    E_D97_RULER_PHASE_A_NONE_POWER_RATE,         /* A���޹�����  */
    E_D97_RULER_PHASE_B_NONE_POWER_RATE,         /* B���޹�����  */
    E_D97_RULER_PHASE_C_NONE_POWER_RATE,         /* C���޹�����  */


    E_D97_RULER_TOTAL_POWER_RATE_FACTOR,         /* �ܹ������� */
    E_D97_RULER_PHASE_A_POWER_RATE_FACTOR,         /* A�๦������ */
    E_D97_RULER_PHASE_B_POWER_RATE_FACTOR,         /* B�๦������ */
    E_D97_RULER_PHASE_C_POWER_RATE_FACTOR,         /* C�๦������ */

    /* ��Ӧ��A5 �α������ݱ�ʶ����� */
    E_D97_RULER_DATE_AND_WEEK_NUMBER,             /* ���ں����� */
    E_D97_RULER_TIME,                             /* ʱ�� */
    E_D97_RULER_STWD_METER_RUN,                      /* ��������״̬�� */
    E_D97_RULER_STWD_NETWIRE_RUN,                  /* ��������״̬�� */
    E_D97_RULER_STWD_WEEK_DAY,                       /* ����������״̬�� */

    E_D97_RULER_METER_HAVA_CONSTANT,              /* �����(�й�) */
    E_D97_RULER_METER_NONE_CONSTANT,              /* �����(�޹�) */

    E_D97_RULER_METER_ID,                         /* ��� */
    E_D97_RULER_USER_ID,                          /* �û��� */
    E_D97_RULER_DEV_CODE,                          /* �豸�� */
    E_D97_RULER_DEMAND_PERIOD,                      /* �����Ҫ������ */
    E_D97_RULER_SLIP_TIME,                          /* ����ʱ�� */
    E_D97_RULER_DISPLAY_INTURN_TIME,              /* ѭ��ʱ�� */
    E_D97_RULER_DISPLAY_HOLD_TIME,                   /* ͣ��ʱ�� */
    E_D97_RULER_DISPLAY_POWER_DECIMAL_DIGITS,      /* ��ʾ����С��λ�� */
    E_D97_RULER_DISPLAY_DEMAND_DECIMAL_DIGITS,      /* ��ʾ����(�������)С��λ�� */
    E_D97_RULER_AUTO_READ_METER_DATE,              /* �Զ��������� */
    E_D97_RULER_LOAD_DELEGATE_DATE,               /* ���ɴ����� */
    E_D97_RULER_HAVE_POWER_START_DATA,            /* �й�������ʼ��ȡ */
    E_D97_RULER_NONE_POWER_START_DATA,            /* �޹�������ʼ��ȡ */
    E_D97_RULER_OUTPUT_PULSE_WIDTH,                  /* ��������� */
    E_D97_RULER_PASSWORD,                         /* ����Ȩ�޼����� */
    E_D97_RULER_YEAR_ZONE_P,                      /* ��ʱ���� P */
    E_D97_RULER_DAY_ZONE_Q,                          /* ��ʱ�α��� q */
    E_D97_RULER_DAY_ZONE_M,                          /* ��ʱ��(ÿ���л���)m <=10*/
    E_D97_RULER_RATE_K,                                /* ������ K<= 14 */
    E_D97_RULER_HOLIDAY_NUMBER_N,                  /* ���������� n */
    E_D97_RULER_START_DATE_DAY_ZONE_P,              /* Pʱ����ʼ���ڼ���ʱ�α��(P = 1~F) */
    E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M,      /* ��(q)��ʱ�ε�(m)ʱ����ʼʱ�估���ʺ� (q=1~8)(m=1-m_max<=10)*/
    E_D97_RULER_DATE_DAY_ZONE_HOLIDAY_N,          /* ��(n)�����������ڼ���ʱ�α�� */
    E_D97_RULER_WEEKS_DAY_ZONE,                        /* �����ղ��õ�ʱʱ�α�� */
    E_D97_RULER_LOAD_RECORD_START_TIME,           /* ���ɼ�¼��ʼʱ�� */
    E_D97_RULER_LOAD_RECORD_PERIOD_TIME,          /* ���ɼ�¼���ʱ�� */    

    /* ��Ӧ��A6���ɼ�¼���ݱ�ʶ����� */
   
    E_D97_RULER_CURRENT_POWER_LOAD_BLOCK,          /* ��ǰ�����й��ܵ��ܸ��ɼ�¼���ݿ� */
    E_D97_RULER_CURRENT_DEMAND_LOAD_BLOCK,           /* ��ǰ��������������ɼ�¼���ݿ� */

    E_D97_RULER_STANDARD_VAR_MAX = E_D97_RULER_CURRENT_DEMAND_LOAD_BLOCK, /* �������ݹ�Լ���� ���ֵ */
    E_D97_RULER_STANDARD_MAX = E_D97_RULER_CURRENT_DEMAND_LOAD_BLOCK, /* dlt645 97 ��׼��Լ�������ֵ */
    
    /* �û���չ */
    E_D97_RULER_USER_EXTEND_MIN,                 /*  �û���չ��Լ������ʼλ�� */

    //.....��������û��Ĺ�Լ����

    E_D97_RULER_USER_EXTEND_MAX,                   /* �û��Զ����Լ���ֵ */


    /* ��֮������û���չ��Լ */
    E_D97_RULER_MAX
    
}E_D97_RULER_TYPE;


/* �������Ͷ��� */
typedef enum
{
    E_D97_OK = 0,  /* 0 ������ȷ */

    /* �������� */
    E_D97_ERROR_PARA_OUT_RANGE,      /* ����Խ�� */
    E_D97_ERROR_PARA_POINTER_NULL,   /* ����ָ��Ϊ�� */
    E_D97_ERROR_PARA_NONE,            /* ������ */
    E_D97_ERROR_ID_UNKNOWN,          /* δ֪�Ĺ�Լ ID ��ԼID û�ж��� */

    //check frame
    E_D97_ERROR_FRAME_UNCOMP,        /* ��������֡���� */
    E_D97_ERROR_FRAME_0x68,          /* ����֡֡��ʼ�� 0x68 ��λ�ò��� */
    E_D97_ERROR_FRAME_CHECK_SUM,     /* ����֡����Ͳ��� */
    E_D97_ERROR_FRAME_END_0x16,      /* ����֡������ 0x16 ���� */

}eD97Err;

/* DLT645 1997 ���Լ��������ָ������ */    
typedef eD97Err(*F_D97_RULER_TRANS)(E_D97_TRANS_FLAG, void*, void*);


/* �鿴���ݵ�ʱ��� */
typedef enum
{
    E_D97_CHECK_POINT_CURRENT,             /* ��ǰ */
    E_D97_CHECK_POINT_LAST_MONTH,          /* ���� */
    E_D97_CHECK_POINT_LAST_LAST_MONTH,   /* ������ */
    
}E_D97_CHECK_POINT;


/* ֡���к��е��������� ��Ͻṹ

 ˵��:    
 (1)XXXXXX.XX �������ֵ������ֵ��С��λ
 (2)NNNN Ϊ����ֵ���趨ֵ
 (3)YYMMDDWW Ϊ�ꡢ�¡��ա��ܴ�
 (4)hhmmss Ϊʱ���֡���

  ÿ�����Ͷ�Ӧһ�ֽṹ���������������װת������
*/
typedef enum
{
    E_D97_FORMAT_UNKOWN,
    /* ʱ�� */
    E_D97_FORMAT_MMDDHHmm,
    E_D97_FORMAT_YYMMDDWW,
    E_D97_FORMAT_hhmmss,
    E_D97_FORMAT_DDhh,

    /* ��� */
    E_D97_FORMAT_MMDDNN,
    E_D97_FORMAT_MMDDhhmm,
    E_D97_FORMAT_hhmmNN,
    
    
    /* NN */
    E_D97_FORMAT_NN,
    E_D97_FORMAT_NN_1 = E_D97_FORMAT_NN,
    
    E_D97_FORMAT_NNNN,
    E_D97_FORMAT_NN_2 = E_D97_FORMAT_NNNN,
    
    E_D97_FORMAT_NNNNNN,
    E_D97_FORMAT_NN_3 = E_D97_FORMAT_NNNNNN,
    
    E_D97_FORMAT_NNNNNNNN,    
    E_D97_FORMAT_NN_4 = E_D97_FORMAT_NNNNNNNN,

    E_D97_FORMAT_NNNNNNNNNN,    
    E_D97_FORMAT_NN_5 = E_D97_FORMAT_NNNNNNNNNN,
    
    E_D97_FORMAT_NN_6,
    E_D97_FORMAT_NN_7,
    E_D97_FORMAT_NN_8,
    E_D97_FORMAT_NN_9,
    
    E_D97_FORMAT_NNNNNNN_N,  /* NNNNNN.N */
    
    /* XX.XX */
    E_D97_FORMAT_X,          /* X */
    E_D97_FORMAT_XX,          /* X */
    E_D97_FORMAT_XXX,        /* XXX */
    E_D97_FORMAT_X_XXX,      /* X.XXX */
    E_D97_FORMAT_XX_XX,      /* XX.XX */
    E_D97_FORMAT_XX_XXXX,    /* XX.XXXX */
    E_D97_FORMAT_XXXXXX_XX,  /* XXXXXX.XX */

    /* STATE WORD */
    E_D97_FORMAT_STWD_METER_RUN,       /* �������״̬�� */
    E_D97_FORMAT_STWD_NETWIRE_RUN,     /* ����״̬�� */
    E_D97_FORMAT_STWD_WEEK_DAY,        /* ������״̬�� */
    E_D97_FORMAT_MAX,

}E_D97_DATA_FORMAT;

/* ��Լ�Ķ�дȨ�� */
typedef enum
{
    E_D97_RDWR_READ_ONLY,   /* ֻ������ */
    E_D97_RDWR_WRITE_ONLY,  /* ֻд���� */
    E_D97_RDWR_READ_WRITE,  /* �ɶ���д */

}E_D97_RULER_READ_WRITE;


/* ֡���ݵĵ�λ���� */
typedef enum
{
    E_D97_UNIT_COUNT,             /* ���� */
    E_D97_UNIT_TIME,              /* ���� */
    E_D97_UNIT_FACTOR,            /* ���� */
    E_D97_UNIT_NUMBER,            /* ���� */
    E_D97_UNIT_MMDDHHmm,          /* ��_��_Сʱ_���� */
    E_D97_UNIT_MMDDhhmm,          /* ��_��_Сʱ_���� */
    E_D97_UNIT_YYMMDDWW,          /* �������� */
    E_D97_UNIT_DD,
    E_D97_UNIT_hhmmss,            /* Сʱ �� �� */
    E_D97_UNIT_hh,                /* Сʱ */
    E_D97_UNIT_mm,                /* ���� */
    E_D97_UNIT_ss,                /* �� */
    E_D97_UNIT_ms,                /* ���� */
    E_D97_UNIT_DDhh,              /* �� Сʱ */
    E_D97_UNIT_A,                 /* ���� �� */
    E_D97_UNIT_V,                 /* ��ѹ �� */
    E_D97_UNIT_KW,                  /* �� ǧ���� */
    E_D97_UNIT_KWH,                  /* �� ǧ����ʱ */
    E_D97_UNIT_KVARH,                 
    E_D97_UNIT_X,                  /* XX ����С���� */
    E_D97_UNIT_X_DOT,              /* ���� XX.XX ��С���� */
    E_D97_UNIT_stwd,              /* ״̬�� */
    E_D97_UNIT_ID,                /* ��ţ��û��ţ��豸�� */

    
    
}E_D97_DATA_UNIT;

/* �������� C ֡���ͷ��� */
typedef enum
{
    E_D97_CTRL_DIR_M2S, // ��վ����վ
    E_D97_CTRL_DIR_S2M, // ��վ����վ
    
}E_D97_CTRL_DIR;

/* ��վ�쳣��־ */
typedef enum
{
    E_D97_CTRL_SR_OK, // ��վ����Ӧ��
    E_D97_CTRL_SR_NO, // ��վ�쳣Ӧ��
    
}E_D97_CTRL_SR;

/* ���޺���֡��־ */
typedef enum
{
    E_D97_CTRL_FLW_NONE, //�޺���
    E_D97_CTRL_FLW_HAVE, //�к���
    
}E_D97_CTRL_FLW;

/* ������C ������ */
typedef enum
{
    E_D97_CTRL_FNC_RESERVE                    = 0x00,   /*����*/
    E_D97_CTRL_FNC_READ_DATA                  = 0x01,   /*��ȡ����*/
    E_D97_CTRL_FNC_READ_AFTERDATA            = 0x02,   /*��ȡ��������*/
    E_D97_CTRL_FNC_READ_DATA_RE              = 0x03,   /*�ض�����*/
    E_D97_CTRL_FNC_WRITE_DATA                = 0x04,   /*д����*/
    E_D97_CTRL_FNC_SYNC_TIME                 = 0x08,   /*�㲥Уʱ*/
    E_D97_CTRL_FNC_WRITE_ADDR                = 0x0A,   /*д�豸��ַ*/
    E_D97_CTRL_FNC_MODIFY_BAUD                = 0x0C,   /*����ͨ������*/
    E_D97_CTRL_FNC_MODIFY_PASSWORD           = 0x0F,   /*��������*/
    E_D97_CTRL_FNC_MAXDEMAND_CLEAR           = 0x10,   /*����������� 0*/
    
}E_D97_CTRL_FNC;

/*  ������������Ľ����־ */
typedef enum
{
    E_D97_UNPD_FLG_OK,          /* ������ȷ ���ݿ��� �������������Ĵ� */
    E_D97_UNPD_FLG_ERROR_OK,    /* ������ȷ ���ݿ��� ���������쳣�Ĵ� */
    E_D97_UNPD_FLG_NONE_DATA,   /* û�ж�Ӧ����Լ ���� */
    E_D97_UNPD_FLG_FUNC_NULL,   /* û�н������� */
    E_D97_UNPD_FLG_FUNC_ERROR,  /* �н������������������̳��� */
    E_D97_UNPD_FLG_NONE_STRUCT, /* û�н��������͵��û������ݽṹ */

}E_D97_UNPD_FLG;

typedef enum
{
    E_D97_ST_OK, //��ʾ����
    E_D97_ST_NO, //��ʾ�����������󣬳���
    
}E_D97_ST_OK_NO;

/* �ṹ */
/* DLT645 1997 ��Լ���Ͷ������ */
typedef struct
{
    E_D97_CHECK_POINT checkPoint;       /* ���ʱ��� */

    unsigned char      rate_k;           /* �������� ֵk(1 ~ 14)*/
    unsigned char    year_p;           /* ��ʱ���� p(1~F) */
    unsigned char    holiday_n;       /*����������n(n < 255)*/
    unsigned char      day_q;              /*��ʱ�α���q (1 ~ 8)*/
    unsigned char    interval_m;     /*��ʱ��(ÿ���л���)m (1~10)*/
        
}S_D97_RULER_TYPE_PARA;





/* ��Լ����ϸ��Ϣ */
typedef struct
{
    E_D97_RULER_TYPE         type;       /* ��Լ���� */
    S_D97_RULER_TYPE_PARA    para;       /* ��Լ���͵Ķ������� */      
    E_D97_DATA_FORMAT        format;     /* ֡�����ݸ�ʽ */
    E_D97_DATA_UNIT          unit;       /* ��λ���� */
    E_D97_RULER_READ_WRITE   rdwr;       /* ��д���� */
    
    int                      length;     /* ���ݳ���(�ֽ���) */
    F_D97_RULER_TRANS        func;       /* ת������ */
    char name[D97_NAME_LEN];             /* ���������� */

}S_D97_RULER_INFOR;

typedef struct
{
    unsigned short ruler_id;     /* ��Լ��ǩ�� C0 C1  ���� 0x40E3 */
    unsigned char  ctrl_code;    /* ������ */
    unsigned char  data_len;     /* �������ֽ��� ������Լ���������� */
    char* address;               /* ��ַ */
    char* data;                  /* ���� */

}S_D97_PACK_FRAME;


/* ������C �ṹ���� */
typedef struct
{
    E_D97_CTRL_DIR direct;    // ֡���ͷ��� 
    E_D97_CTRL_SR  reply;      // ��վӦ���쳣��־
    E_D97_CTRL_FLW follow;      // ���޺���֡
    E_D97_CTRL_FNC funcode;      // ������
    
}S_D97_CTRL_CODE;


/* ������Ϣ״̬�� */
typedef struct
{
    E_D97_ST_OK_NO  rate_k;      // E_D97_ST_NO ��ʾ����k����
    E_D97_ST_OK_NO    day_q;      // E_D97_ST_NO ��ʱ������
    E_D97_ST_OK_NO  year_p;     // E_D97_ST_NO ��ʱ������
    E_D97_ST_OK_NO    password;    // E_D97_ST_NO �����
    E_D97_ST_OK_NO  rule_id;    // E_D97_ST_NO ���ݱ�ʶ��
    E_D97_ST_OK_NO    data;        // E_D97_ST_NO �Ƿ�����
    
}S_D97_STWD_ERR;

/* ���״̬�� �û���ṹ */
typedef struct
{
    E_D97_ST_OK_NO dir_none;   // �޹����ܷ���         ����(E_D97_ST_OK) ����(E_D97_ST_NO)
    E_D97_ST_OK_NO dir_have;   // �й����ܷ���         ����(E_D97_ST_OK) ����(E_D97_ST_NO)
    E_D97_ST_OK_NO cell;       // ��ص�ѹ          ����(E_D97_ST_OK) Ƿѹ(E_D97_ST_NO)
    E_D97_ST_OK_NO demand;        // ����������㷽ʽ  ����(E_D97_ST_OK) ����(E_D97_ST_NO)
    E_D97_ST_OK_NO read;       // ����ʽ             �Զ�(E_D97_ST_OK) �ֶ�(E_D97_ST_NO)
    
}S_D97_STWD_METER;

/*��������״̬�� �û���ṹ */
typedef struct
{
    E_D97_ST_OK_NO volt_A;      // A���ѹ  ����(E_D97_ST_OK) ��ѹ(E_D97_ST_NO)
    E_D97_ST_OK_NO volt_B;      // B���ѹ  ����(E_D97_ST_OK) ��ѹ(E_D97_ST_NO) 
    E_D97_ST_OK_NO volt_C;        // C���ѹ  ����(E_D97_ST_OK) ��ѹ(E_D97_ST_NO)
    E_D97_ST_OK_NO out_A;       // A��ϵ�  ����(E_D97_ST_OK) �ϵ�(E_D97_ST_NO)
    E_D97_ST_OK_NO out_B;        // B��ϵ�  ����(E_D97_ST_OK) �ϵ�(E_D97_ST_NO)
    E_D97_ST_OK_NO out_C;        // C��ϵ�  ����(E_D97_ST_OK) �ϵ�(E_D97_ST_NO)
    
}S_D97_STWD_NET_WIRE;

// ������״̬�� �û���ṹ
typedef struct
{
    E_D97_ST_OK_NO day_1;      /* ��1   ����(E_D97_ST_OK) ��Ϣ(E_D97_ST_NO)*/
    E_D97_ST_OK_NO day_2;      /* ��2   ����(E_D97_ST_OK) ��Ϣ(E_D97_ST_NO)*/
    E_D97_ST_OK_NO day_3;      /* ��3   ����(E_D97_ST_OK) ��Ϣ(E_D97_ST_NO)*/
    E_D97_ST_OK_NO day_4;      /* ��4   ����(E_D97_ST_OK) ��Ϣ(E_D97_ST_NO)*/
    E_D97_ST_OK_NO day_5;      /* ��5   ����(E_D97_ST_OK) ��Ϣ(E_D97_ST_NO)*/
    E_D97_ST_OK_NO day_6;      /* ��6   ����(E_D97_ST_OK) ��Ϣ(E_D97_ST_NO)*/
    E_D97_ST_OK_NO day_7;      /* ����  ����(E_D97_ST_OK) ��Ϣ(E_D97_ST_NO)*/

}S_D97_STWD_WEEK_DAY;


typedef struct
{
    unsigned char MM;
    unsigned char DD;
    unsigned char HH;
    unsigned char mm;
    
}S_D97_FMT_MMDDHHmm;



typedef struct
{
    unsigned char YY;
    unsigned char MM;
    unsigned char DD;
    unsigned char WW;
    
}S_D97_FMT_YYMMDDWW;

typedef struct
{
    unsigned char DD;
    unsigned char hh;
    
}S_D97_FMT_DDhh;

typedef struct
{
    unsigned char MM;
    unsigned char DD;
    unsigned char NN;
}S_D97_FMT_MMDDNN;

typedef struct
{
    unsigned char hh;
    unsigned char mm;
    unsigned char ss;    
    
}S_D97_FMT_hhmmss;

/* �û��ӿڲ����ݽṹ */
typedef union
{



    /* ״̬�� */
    S_D97_STWD_ERR      stwd_err; // ������Ϣ״̬��
    S_D97_STWD_WEEK_DAY weed_day; // ������״̬��
    S_D97_STWD_NET_WIRE net_wire; // ��������״̬��
    S_D97_STWD_METER     meter;    // �������״̬��


}U_D97_DATA_UNPACK;


/* ����֡��Ϣ���ݽṹ */
typedef struct
{
    unsigned char          ctrl_c;           /* ������ �ֽ��� */
    unsigned char          lead_num;         /* ǰ���ַ� 0xFE�ĸ��� */
    unsigned short         ruler_id;         /* ��ԼID */
    unsigned short           data_len;         /* ������ */
    unsigned short         frame_len;         
    
    E_D97_RULER_TYPE       type;             /* ��Լ���� */
    E_D97_DATA_FORMAT      format;           /* ֡�����ݸ�ʽ */
    E_D97_DATA_UNIT        unit;             /* ��λ���� */
    E_D97_UNPD_FLG         flag;             /* �������ݵ��û������ݵı�־��˵������״̬ ����data_unpack �Ƿ���� */
    S_D97_CTRL_CODE        ctrl_s;           /* ������ṹ */
    S_D97_RULER_TYPE_PARA  para;             /* ��Լ���͵Ķ������� */ 
    
    U_D97_DATA_UNPACK      data_unpack;      /* ת��Ϊ�û�������� */ 
    unsigned char          data_pack[D97_DATA_MAX_NR];   /* ������ ��������ԼID�Ĳ��� û����ת�� */
    unsigned  char         address[D97_ADDR_LEN+1]; //12λ��ַ������         
}S_D97_UNPACK;



/* �������� */
/* ͨ����ԼID��ȡ��Լ������ϸ��Ϣ */
eD97Err get_d97_ruler_info(unsigned short rulerID, S_D97_RULER_INFOR *outRulerInfo);

/* ͨ����Լ���� �� һЩ����Ĳ�������ù�Լ ID */
eD97Err get_d97_ruler_id(E_D97_RULER_TYPE rulerType,  S_D97_RULER_TYPE_PARA para, unsigned short *poutRulerID);

//eD97Err get_d97_ruler_type();

/* ͨ����ַ ��ԼID ���������ݼ����� ��֡ */
eD97Err pack_d97_frame_by_data(S_D97_PACK_FRAME *inPara, char *outBuffer, int *outLength);  

//���� dlt645 97֡ 
eD97Err unpack_d97_frame(void *inpBuffer, int inBufferLength, S_D97_UNPACK *outpFrame);

/* �������ݸ�ʽ����ת��֡���� */
eD97Err trans_d97_frame_data(E_D97_DATA_FORMAT format,
                            E_D97_TRANS_FLAG flag,
                            void *pUser,
                            void *pFrame);

/* ��һ����Ч֡���Һ��� */
int get_d97_first_valid_frame(void *inpBuffer,  // ����buffer
                              int inBufferLen,  // ����buffer�ֳ�
                              char** poutPos,   // ��һ����Ч֡��λ��
                              int * poutLen);   // ��һ����Ч֡�ĳ���
/* �ж�һ��֡�ǲ�����ȷ��dlt645 1997 */
int is_right_d97_frame(void *inpBuffer, int inBufferLen);

/* ����ķֽ�ת������ */
eD97Err trans_d97_format_X(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_XX(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_XXX(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_X_XXX(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_XX_XX(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_XX_XXXX(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_XXXXXX_XX(E_D97_TRANS_FLAG flag, void *user, void*frame);

/*YYMMDDhhmmss*/
eD97Err trans_d97_format_MMDDHHmm(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_YYMMDDWW(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_hhmmss(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_DDhh(E_D97_TRANS_FLAG flag, void *user, void*frame);

/* state word transfer function */
eD97Err trans_d97_format_stwd_meter(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_stwd_netwire(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_stwd_weekday(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_stwd_error(E_D97_TRANS_FLAG flag, void *user, void*frame);

eD97Err trans_d97_format_MMDDNN(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_MMDDhhmm(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_hhmmNN(E_D97_TRANS_FLAG flag, void *user, void*frame);

eD97Err trans_d97_format_NN(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_NNNN(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_NNNNNN(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_NNNNNNNN(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_NN_6(E_D97_TRANS_FLAG flag, void *user, void*frame);
eD97Err trans_d97_format_NNNNNN_N(E_D97_TRANS_FLAG flag, void *user, void*frame);

// ����״̬��
eD97Err trans_d97_format_err_word(E_D97_TRANS_FLAG flag, void *user, void*frame);



/*�ڲ�����*/
void d97_str2bcd(const char *pstr, unsigned char *pbcd, int len);
void bcd2str(const char* inBCD, char *outStr, int len);       /* BCD ת��Ϊ �ַ��� */
double d97_bcd_to_decimal(unsigned char* pBcd, int len);
int  d97_remove_dot(int len, char*inArray, char *outArray);
void d97_bcd2str(const char* inBCD, char *outStr, int len);
double d97_bcd_to_double(unsigned char* pBcd, int len,int LeftDigit);
/* ���������� */
eD97Err trans_d97ctrl_struct2char(unsigned char *outChar, S_D97_CTRL_CODE *inStruct); //���ṹ��װ���ֽ�
eD97Err trans_d97ctrl_char2struct(unsigned char inChar,  S_D97_CTRL_CODE *outStruct); //���ֽڷ�װ�ɽṹ

#if 0




/* ��װ����װ */

/* frame */
int pack_d97_frame_by_id(); /* */
int pack_d97_frame_by_type();

/* data */
int pack_d97_data_field();
int unpack_d97_data_field();






#endif 


#endif /* _DLT645_API_1997_H_ */


