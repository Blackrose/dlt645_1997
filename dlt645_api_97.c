#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>
#include "dlt645_api_97.h"



#ifdef D97_DEBUG_ON

int g_d97_need_pack = D97_OFF;
char g_d97_out_data[OUT_BUF_LEN] = {0};


void debug_switch_d97(int show)
{
     g_d97_need_pack = show;
}
#endif





/* ȫ�ֱ��� */
UINT8 g_year_p     = YEAR_P_MAX;       /* ��ʱ���� P(1~F)*/
UINT8 g_interval_m = INTERVAL_M_MAX;   /*��ʱ��(ÿ���л���)m (1~10)*/
UINT8 g_day_q      = DAY_Q_MAX;        /*��ʱ�α��� q (1~8)*/
UINT8 g_rate_k     = RATE_K_MAX;       /* ������ k (1~14)*/
UINT8 g_holiday_n  = HOLIDAY_N_MAX;    /* ���������� n*/


/* ����ʵ�� */

/*************************************************
Function:       d97_str_to_num
Description:   ������Ϊlen��stringתΪ����ת��
Calls:          
Called By:      
Input:          

Output:        

Return:        
Others:     

*************************************************/
int d97_str_to_num(char* str, int len)
{
    char buf[10] = {0};
    int i;
    
    if(len > 10)
    {
        return -1;
    }
    
    for(i = 0; i < len; i++)
    {
        buf[i] = str[i];        
    }
    
    return atoi(buf);
}
/*************************************************
Function:       d97_bcd2str
Description:   ������Ϊlen��BCD��תΪ�ַ���ת��
Calls:          
Called By:      
Input:          

Output:        

Return:        
Others:     

*************************************************/
void d97_bcd2str(const char* inBCD, char *outStr, int len)
{
    int i = 0,j= 0;
    char c1 = 0,c0 = 0;
    
    if(NULL == inBCD || NULL == outStr || len < 1)
    {
        return;
    }

    for(i = len - 1,j = 0; i >= 0; i--)
    {
        c0 = inBCD[i] & 0xF;
        c1 = (inBCD[i] >> 4) & 0xF;

        if(c1 >= 0 && c1 <= 9)
        {
           outStr[j++] = c1 + '0';    
          
        }
        else
        {
           outStr[j++] = c1 + 'A';
        }
        
        if(c0 >= 0 && c0 <= 9)
        {
           outStr[j++] = c0 + '0';    
          
        }
        else
        {
           outStr[j++] = c0 + 'A';
        }    
    }
    
}




/*************************************************
Function:       d97_remove_dot
Description:    ���ַ������е�.ȥ�����ϳɳ���Ϊlen��������
Calls:          
Called By:      
Input:          

Output:        

Return:        
Others:     

*************************************************/

int d97_remove_dot(int len, char*inArray, char *outArray)
{
    int i = 0;
    int j = 0;

    if(NULL == inArray || NULL == outArray)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    for(i = 0; j < len; i++)
    {
        if((inArray[i] < '0')
            ||((inArray[i] > '9') && (inArray[i] < 'A')) 
            ||((inArray[i] > 'F') && (inArray[i] < 'a')) 
            ||(inArray[i] > 'f'))
        {
            
            continue;
        }

        outArray[j++]= inArray[i];
    }

    outArray[j] = 0;
    return E_D97_OK;
}
/*************************************************
  Function:       d97_BCD2Decimal
  Description:    ��bcd��ת��Ϊʮ������
  Calls:          
  Called By:      
  Input:          UINT8* pBcd, UINT8 len
                
  Output:         
                    
  Return:         ���ذ����ܳ���ֵ
  Others:         ���֧��λ�� 20, �����Ϊ 18446744073709551616
*************************************************/
double d97_bcd_to_decimal(UINT8* pBcd, int len)
{
      int  i = 0;
    double dDeci = 0.0;
    
    for(i = len - 1; i >= 0; i--)
    {
        dDeci += (pBcd[i] >> 4) * 10;
        dDeci += (pBcd[i] & 0x0f);
        if(i != 0)
            dDeci *= 100;
    
    }    
    
    return dDeci;
}


int d97_pow_r(int value,int x)
{
    int tmp = 1;
    while(x > 0){
        tmp *= value;
        x--;
    }
    return tmp;
}

/*************************************************
  Function:       d97_bcd_to_double
  Description:    dlt645 1997 ��bcd��ת��Ϊʮ����������
  Calls:          
  Called By:      
  Input:          UINT8* pBcd, UINT8 len
                    UINT8 LeftDigit С��������λ��
                
  Output:         
                    
  Return:         ���ذ����ܳ���ֵ
  Others:         ���֧��λ�� 20, �����Ϊ 18446744073709551616
                  �����λ��Ϊ0x8X ������Ǹ���
*************************************************/
double d97_bcd_to_double(UINT8* pBcd, int len,int LeftDigit)
{
    UINT8 ucPlus;
    int flag = 1;
    double d = 0.0;
    ucPlus = pBcd[len-1];
    

    if(ucPlus & 0x80)
    {
        pBcd[len-1] &= 0x0F;
        flag = -1;
    }

    d = d97_bcd_to_decimal(pBcd, len);
    d = d/d97_pow_r(10, LeftDigit);
    d = d*flag;

    return d;

}

/*************************************************
Function:       d97_str2bcd
Description:    ������Ϊlen���ַ���pstr��ΪBCD�����pbcd
Calls:          
Called By:      
Input:          

Output:        

Return:        
Others:       pstr�ĳ�����pbcd�Ķ��� 

*************************************************/
void d97_str2bcd(const char *pstr, UINT8 *pbcd, int len)
{
    UINT8 tmpValue;
    int i;
    int j;
    int m;
    int sLen;
    
    sLen = strlen(pstr);
    for(i = 0; i < sLen; i++)
    {
        if((pstr[i] < '0')
            ||((pstr[i] > '9') && (pstr[i] < 'A')) 
            ||((pstr[i] > 'F') && (pstr[i] < 'a')) 
            ||(pstr[i] > 'f'))
        {
            sLen=i;
            break;
        }
    }

    sLen = (sLen <= (len * 2) ) ?  sLen : sLen * 2;
    memset((void *)pbcd, 0x00, len);
    
    for(i=sLen-1, j=0, m=0; (i>=0)&&(m<len); i--, j++)
    {
        if((pstr[i] >= '0') && (pstr[i] <= '9'))
        {
            tmpValue=pstr[i] - '0';
        }
        else if((pstr[i] >= 'A') && (pstr[i] <= 'F'))
        {
            tmpValue=pstr[i] - 'A' + 0x0A;
        }
        else if((pstr[i] >= 'a') && (pstr[i] <= 'f'))
        {
            tmpValue=pstr[i] - 'a' + 0x0A;
        }
        else
        {
            tmpValue=0;
        }
        
        if((j%2)==0)
        {
            pbcd[m]=tmpValue;        
        }
        else
        {
            pbcd[m++]|=(tmpValue << 4);
        }
        
        if((tmpValue==0) && (pstr[i] != '0'))
        {
            break;
        }
    }
}


void bcd2str(const char* inBCD, char *outStr, int len)
{
}


/*************************************************
Function:       unpack_d97_frame
Description:    ����DLT645 1997֡���ܺ���

Author:         liming 

Calls:          
Called By:      
Input:          inpBuffer      �������֡bufferָ��
                inBufferLength ��buffer����
                
Output:         outpFrame ��֡�ĸ�����ϸ��Ϣ

Return:         ��ȷ����0

Others:         ��Ҫ�Ĺ��ܺ���
  
*************************************************/
eD97Err unpack_d97_frame(void *inpBuffer, int inBufferLength, S_D97_UNPACK *outpFrame)
{

    int nNumber0xFE = 0;                     /* ǰ���ַ��ĸ��� */
    int i = 0;
    int pos = 0;
    int nCheckSumPosStart, nCheckSumPosEnd;
    int ret = 0;
    UINT8 ucCtrlCode = 0;
    UINT8 ucDataLen = 0;
    UINT8 ucCheckSum = 0;
    UINT16 usRulerID = 0;
    UINT8 *buffer = (UINT8 *)inpBuffer;
    char addr_bcd[6] = {0};                      //��ַ��Ϣ(BCD��ʽ)
    char addr_str[D97_ADDR_LEN+1] = {0};         //��ַ��Ϣ(string��ʽ)
    UINT8 aucDataTmp[D97_DATA_MAX +1];
    S_D97_RULER_INFOR stRulerInfor = {0};
    F_D97_RULER_TRANS pTransFunc;    /* ��������ָ�� */


    if(NULL == inpBuffer || NULL == outpFrame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(inBufferLength < D97_FRAME_LEN_MIN)
    {
        return E_D97_ERROR_FRAME_UNCOMP;
    }


    for(i = 0; i < 4; i++)
    {
        if(0xFE == buffer[i])
        {
            pos++;
            nNumber0xFE++;
        }
    }

    nCheckSumPosStart = pos;
        
    // ���ǰ���ַ� 0x68
    if(0x68 != buffer[pos] || 0x68 != buffer[pos+7])
    {
        return E_D97_ERROR_FRAME_0x68;
    }

    //��ַ
    pos++;
    for(i = 0; i < 6; i++)
    {
        addr_bcd[i] = buffer[pos++];
    }

    pos++;
    d97_bcd2str(addr_bcd, addr_str, 6);

    //������
    ucCtrlCode = buffer[pos++];

    //ת��������
    ret =  trans_d97ctrl_char2struct(ucCtrlCode, &(outpFrame->ctrl_s));
    if(E_D97_OK != ret)
    {
        return ret;
    }

    //�����򳤶�
    ucDataLen = buffer[pos++];
    
    for(i = 0; i < ucDataLen; i++)
    {
        aucDataTmp[i] = buffer[pos++] - 0x33; 
    }

    nCheckSumPosEnd = pos;

    //���checksum
    for(i = nCheckSumPosStart; i < nCheckSumPosEnd; i++)
    {
        ucCheckSum +=  buffer[i];
    }

    if(ucCheckSum != buffer[pos++])
    {
        return E_D97_ERROR_FRAME_CHECK_SUM;
    }

    //������
    if(0x16 != buffer[pos++])
    {
        return E_D97_ERROR_FRAME_END_0x16;
    }

    //��ַ
    for(i = 0; i < D97_ADDR_LEN; i++)
    {
        outpFrame->address[i] = addr_str[i];
    }

    
    // �����ı���
    if(E_D97_CTRL_SR_OK == outpFrame->ctrl_s.reply)
    {
        // ԭ���� 
        for(i = 2; i < ucDataLen; i++)
        {
            outpFrame->data_pack[i-2] = aucDataTmp[i];
        }
    
        // ��ԼID
        usRulerID = (aucDataTmp[0] & 0xFF) | ((aucDataTmp[1] << 8) & 0xFF00);
        
        // ͨ��ID��ȡ��ϸ��Ϣ    
        ret = get_d97_ruler_info(usRulerID, &stRulerInfor);

        if(E_D97_OK != ret)
        {
            return ret;
        }
        
        if(NULL == stRulerInfor.func)
        {
            outpFrame->flag    = E_D97_UNPD_FLG_FUNC_NULL;
        }
        else if(ucDataLen-2 > 0)
        {
            
            pTransFunc          = stRulerInfor.func;
            ret = pTransFunc(E_D97_TRANS_F2U, 
                            (void*)&(outpFrame->data_unpack),
                            (void*)&(outpFrame->data_pack));
            if(E_D97_OK == ret)
            {
                outpFrame->flag    = E_D97_UNPD_FLG_OK;
            }
            else
            {
                outpFrame->flag =  E_D97_UNPD_FLG_FUNC_ERROR;
            }
            
            
        }

        outpFrame->data_len  = ucDataLen - 2;
    }
    else
    {
        for(i = 0; i < ucDataLen; i++)
        {
            outpFrame->data_pack[i] = aucDataTmp[i];
        }
        // �쳣����
        outpFrame->data_len  = ucDataLen;
        outpFrame->flag    = E_D97_UNPD_FLG_OK;        
        ret = trans_d97_format_err_word(E_D97_TRANS_F2U, 
                    (void*)&(outpFrame->data_unpack),
                    (void*)&(outpFrame->data_pack));

        if(E_D97_OK == ret)
        {
            outpFrame->flag    = E_D97_UNPD_FLG_ERROR_OK;
        }
        else
        {
            outpFrame->flag =  E_D97_UNPD_FLG_FUNC_ERROR;
        }    
    
    }
    
    /* ��װ��������Ϣ */
    outpFrame->lead_num  = nNumber0xFE;
    outpFrame->ctrl_c      = ucCtrlCode;
    outpFrame->ruler_id  = usRulerID;
    
    outpFrame->frame_len = ucDataLen + 12;
    outpFrame->type      = stRulerInfor.type;
    outpFrame->format    = stRulerInfor.format;
    outpFrame->unit      = stRulerInfor.unit;

    return E_D97_OK;
}


/*************************************************
Function:       pack_d97_frame_by_data
Description:    �ڲ�����������ɽṹ������

Author:         liming 

Calls:          
Called By:      
Input:          S_D97_PACK_FRAME *inPara ���ڷ�֡����
                
Output:         char *outBuffer ��װ���֡buffer
                int  *outLength ��֡�ܳ�
Return:         ��ȷ����0

Others:         
  
*************************************************/
eD97Err pack_d97_frame_by_data(S_D97_PACK_FRAME *inPara, char *outBuffer, int *outLength)
{
    int i;
    int len = 0; /* ����������ֽڳ� */
    UINT8 ucCheckSum = 0;
    UINT8 ucDi0,ucDi1;
    UINT8 aucAddrTmp[6]; 

    if(NULL == outBuffer || NULL == inPara ||  NULL == outLength)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    // ׼������ 
    ucDi0 = inPara->ruler_id & 0xFF;
    ucDi1 = (inPara->ruler_id >> 8) & 0xFF;

    d97_str2bcd(inPara->address, aucAddrTmp, 6);

    /* ��ʼ��֡ */
    // 1 ֡��ʼ��
    outBuffer[len++] = 0x68; 

    // 2 ��ַ�� 
    for(i = 0; i < 6; i++)
    {
        outBuffer[len++] = aucAddrTmp[i];
    }
    
    // 3 ֡��ʼ��
    outBuffer[len++] = 0x68; 

    // 4 ������
    outBuffer[len++] = inPara->ctrl_code; 

    // 5 �������ֽ���
    outBuffer[len++] = inPara->data_len;

    // 6 ��ԼID  - ���������� ���ֽڼ�0x33
    outBuffer[len++] = ucDi0 + 0x33;
    outBuffer[len++] = ucDi1 + 0x33;

    // 7 ��������
    for(i = 0; i < inPara->data_len - 2; i++)
    {
       outBuffer[len++] = inPara->data[i] + 0x33;
    }

    // 8 ���������
    for(i = 0; i < len; i++)
    {
        ucCheckSum += outBuffer[i];
    }

    // 9 �����
    outBuffer[len++] = ucCheckSum;

    // 10 ������
    outBuffer[len++] = 0x16;

    /* ������� */
    *outLength = len;

    return E_D97_OK;
}


/*************************************************
Function:       get_d97_ruler_info
Description:    ͨ����ԼID��ö�Ӧ��Լ����ϸ��Ϣ:

                (1)��Լ�����ͼ���������
                (2)��Ӧ���ݸ�ʽ����
                (3)�����򳤶�
                (4)����������ݵ�λ
                (5)��Լ�Ŀɶ�д����
                (6)�ù�Լ������������
                (7)ʵ�ֶԸ�����Լ�����ͷ�װ����ָ��

Author:         liming 

Calls:          
Called By:      
Input:          UINT16 rulerID ��ԼID
                
Output:         S_D97_RULER_INFOR *outRulerInfo
Return:         ��ȷ���� 0 ����Ϊ��������

Others:         ��Ҫ��������Լ�������ĺ���
  
*************************************************/
eD97Err get_d97_ruler_info(UINT16 rulerID, S_D97_RULER_INFOR *outRulerInfo)
{
    UINT8 ucDi11, ucDi10, ucDi01, ucDi00;

    S_D97_RULER_TYPE_PARA    para = {0};      /* ��Լ���͵Ķ������� */      
    E_D97_RULER_TYPE         type;                
    E_D97_DATA_FORMAT         format;          
    E_D97_DATA_UNIT          unit;              
    E_D97_RULER_READ_WRITE     rdwr;              
    
    int                      length;          
    F_D97_RULER_TRANS         func;              
    
    char                      name[128] = {0}; // ��������
    char                     *pName = name;    

    if(NULL == outRulerInfo)
    {
       return E_D97_ERROR_PARA_POINTER_NULL;
    }

    ucDi00 = rulerID & 0xF;
    ucDi01 = (rulerID >> 4) & 0xF;
    ucDi10 = (rulerID >> 8) & 0xF;
    ucDi11 = (rulerID >> 12) & 0xF;
    
    switch(ucDi11)
    {
        /* ��������� */
        case 9: /* {9}[*][*][*] */
        {
            switch(ucDi10) 
            {
                case 0: /* [9]{0}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_CURRENT; /* ����Ϊ:���� */
                    
                    switch(ucDi01)
                    {
                        case 1:/* [9][0]{1}[*] */
                        {
                            if(0 == ucDi00)       /* [9][0][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_POWER_TOTAL;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                pName     =  "(��ǰ)�����й��ܵ���";     
                            }
                            else if(0xF == ucDi00) /* [9][0][1]{F} */
                            {
                            
                                type    =  E_D97_RULER_FORTH_HAVE_POWER_BLOCK;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                 
                                pName     =  "(��ǰ)�����й��������ݿ�";              

                            }
                            else                 /* [9][0][1]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_HAVE_POWER_RATE;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func    =  trans_d97_format_XXXXXX_XX;               
                                sprintf(name, "(��ǰ)����%d�����й��ܵ���", ucDi00);     
                            }
                        }
                        break;

                        case 2:  /* [9][0]{2}[*] */
                        {
                            if(0 == ucDi00)        /* [9][0][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_POWER_TOTAL;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                pName     =  "(��ǰ)�����й��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][0][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_POWER_BLOCK;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                    
                                pName     =  "(��ǰ)�����й��������ݿ�";              
                            }
                            else    /* [9][0][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_HAVE_POWER_RATE;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                sprintf(name, "(��ǰ)����%d�����й��ܵ���", ucDi00);  
                            }
                        }
                        break;

                        default:            /* [9][0]{!(1-2)[*]} */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;
             
                case 1:   /* [9]{1}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_CURRENT;
                    
                    switch(ucDi01) 
                    {
                        case 1:        /* [9][1]{1}[*] */
                        {
                            if(0 == ucDi00)         /*[9][0][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_POWER_TOTAL;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                pName     =  "(��ǰ)�����޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)     /*[9][0][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_POWER_BLOCK;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                    
                                pName     =  "(��ǰ)�����޹��������ݿ�";                

                            }
                            else                    /*[9][0][1]{1~E} */ 
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_NONE_POWER_RATE;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                sprintf(name, "(��ǰ)������%d���޹��ܵ���", ucDi00); 
                            }
                        }
                        break;

                        case 2:        /* [9][1]{2}[*] */
                        {
                            if(0 == ucDi00)        /* [9][1][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_POWER_TOTAL;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                pName     =  "(��ǰ)�����޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][1][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_POWER_BLOCK;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                    
                                pName     =  "(��ǰ)�����޹��������ݿ�";              
                            }
                            else                /* [9][1][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                        
                                unit     =  E_D97_UNIT_KWH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                sprintf(name, "(��ǰ)����%d�����޹��ܵ���", ucDi00);     
                            
                            }

                        }
                        break;
                        
                        case 3:/* [9][1]{3}[*] */
                        {
                            if(0 == ucDi00)/* [9][1][3]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                length     =  4;                                          
                                func     =  trans_d97_format_XXXXXX_XX;                
                                pName     =  "(��ǰ)һ�����޹��ܵ���";                                                
                            }
                            else if(0xF == ucDi00)/* [9][1][3]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                               
                                func     =  NULL;                                       
                                pName     =  "(��ǰ)һ�����޹��������ݿ�";                
                            }
                            else                /* [9][1][3]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_1_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                 
                                sprintf(name, "(��ǰ)����%dһ�����޹��ܵ���", ucDi00);                                 
                            }
                        }
                        break;

                        case 4:/* [9][1]{4}[*] */
                        {
                            if(0 == ucDi00)    /* [9][1][4]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(��ǰ)�������޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][1][4]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                     
                                pName     =  "(��ǰ)�������޹��������ݿ�";                
                            }
                            else    /* [9][1][4]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_4_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                 
                                sprintf(name, "(��ǰ)����%d�������޹��ܵ���", ucDi00);                              
                            }
                        }
                        
                        break;

                        case 5:/* [9][1]{5}[*] */
                        {
                            if(0 == ucDi00)                /* [9][1][5]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                    
                                rdwr     =  E_D97_RDWR_READ_ONLY;                  
                                length    =  4;                             
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(��ǰ)�������޹��ܵ���";                                             
                            }
                            else if(0xF == ucDi00)        /* [9][1][5]{F} */
                        {
                                type     =  E_D97_RULER_QUAD_2_NONE_POWER_BLOCK;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                        
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  UNKNOWN_LEN;                             
                                func    =  NULL;                                     
                                pName   =  "(��ǰ)�������޹��������ݿ�";                

                            }
                            else    /* [9][1][5]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_2_NONE_POWER_RATE;      
                                format  =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit    =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func    =  trans_d97_format_XXXXXX_XX;                 
                                sprintf(name, "(��ǰ)����%d�������޹��ܵ���", ucDi00);          
                            }
                        }
                        break;

                        case 6:/* [9][1]{6}[*] */
                        {
                            if(0 == ucDi00)/* [9][1][6]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_POWER_TOTAL;        
                                format  =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                  
                                pName     =  "(��ǰ)�������޹��ܵ���";                                                
                            }
                            else if(0xF == ucDi00)/* [9][1][6]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                     
                                pName     =  "(��ǰ)�������޹��������ݿ�";                
                            }
                            else    /* [9][1][6]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_3_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                 
                                sprintf(name, "(��ǰ)����%d�������޹��ܵ���", ucDi00);            
                            }
                        }
                        break;

                        default:     /* [9][1]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;
            
#if 0
                case 2:  /* [9][Di10 = 2][*][*] */
                {

                }
                break;

                case 3: /* [9][Di10 = 3][*][*] */
                {

                }
                break;
#endif

                case 4: /* [9]{4}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_LAST_MONTH;

                    switch(ucDi01)
                    {
                        case 1:/* [9][4]{1}[*] */
                        {
                            if(0 == ucDi00)         /* [9][4][1]{0} */
                            {
                                type    =  E_D97_RULER_FORTH_HAVE_POWER_TOTAL;    
                                format  =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                pName     =  "(����)�����й��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)     /* [9][4][1]{F} */
                            {
                                type    = E_D97_RULER_FORTH_HAVE_POWER_BLOCK;   
                                format     =  E_D97_FORMAT_XXXXXX_XX;                 
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                 
                                pName   =  "(����)�����й��������ݿ�";              
                            }
                            else                      /* [9][4][1]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_HAVE_POWER_RATE;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                sprintf(name, "(����)����%d�����й��ܵ���", ucDi00);          
                            }
                        }
                        break;

                        case 2:      /* [9][4]{2}[*] */
                        {
                            if(0 == ucDi00)        /* [9][4][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_POWER_TOTAL;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                pName     =  "(����)�����й��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][4][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_POWER_BLOCK;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                    
                                pName     =  "(����)�����й��������ݿ�";              
                            }
                            else    /* [9][4][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_HAVE_POWER_RATE;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                sprintf(name, "(����)����%d�����й��ܵ���", ucDi00);            
                            }
                        }
                        break;

                        default:     /* [9][4]{!(1-2)}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                case 5: /* [9]{5}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_LAST_MONTH;   /* ���� */
                        
                    switch(ucDi01) 
                    {
                        case 1:    /* [9][5]{1}[*] */
                        {
                            if(0 == ucDi00)         /* [9][5][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_POWER_TOTAL;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                pName     =  "(����)�����޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00) /*[9][5][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_POWER_BLOCK;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                    
                                pName     =  "(����)�����޹��������ݿ�";              

                            }
                            else                 /* [9][5][1]{1~E} */ 
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_NONE_POWER_RATE;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                sprintf(name, "(����)����%d�����޹��ܵ���", ucDi00);          
                            }
                        }
                        break;

                        case 2:/* [9][5]{2}[*] */
                        {
                            if(0 == ucDi00)/* [9][1][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                        
                                unit     =  E_D97_UNIT_KWH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                pName     =  "(����)�����޹��ܵ���";                                                     
                            }
                            else if(0xF == ucDi00)/* [9][5][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                        
                                unit     =  E_D97_UNIT_KWH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                               
                                func     =  NULL;                                        
                                pName     =  "(����)�����޹��������ݿ�";                  

                            }
                            else    /* [9][5][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                        
                                unit     =  E_D97_UNIT_KWH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                sprintf(name, "(����)����%d�����޹��ܵ���", ucDi00);              
                            }

                        }
                        break;
                        
                        case 3:/* [9][5]{3}[*] */
                        {
                            if(0 == ucDi00)/* [9][5][3]{0} */
                            {    
                                type     =  E_D97_RULER_QUAD_1_NONE_POWER_TOTAL;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(����)һ�����޹��ܵ���";                                                  
                            }
                            else if(0xF == ucDi00)/* [9][5][3]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(����)һ�����޹��������ݿ�";                 
                            }
                            else    /* [9][5][3]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_1_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                sprintf(name, "(����)����%dһ�����޹��ܵ���", ucDi00);         
                            }
                        }
                        break;

                        case 4:/* [9][5]{4}[*] */
                        {
                            if(0 == ucDi00)/* [9][5][4]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(����)�������޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][5][4]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(����)�������޹��������ݿ�";                 

                            }
                            else    /* [9][5][4]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_4_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                sprintf(name, "(����)����%d�������޹��ܵ���", ucDi00);         
                            }

                        }
                        break;

                        case 5:/* [9][5]{5}[*] */
                        {
                            if(0 == ucDi00)/* [9][5][5]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(����)�������޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][5][5]{F} */
                            {
                                
                                type     =  E_D97_RULER_QUAD_2_NONE_POWER_BLOCK;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(����)�������޹��������ݿ�";                 

                            }
                            else    /* [9][5][5]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                
                                type     =  E_D97_RULER_QUAD_2_NONE_POWER_RATE;      
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                sprintf(name, "(����)����%d�������޹��ܵ���", ucDi00);         
                            }

                        }
                        break;

                        case 6:/* [9][5]{6}[*] */
                        {
                            if(0 == ucDi00)/* [9][5][6]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(����)�������޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][5][6]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(����)�������޹��������ݿ�";                 

                            }
                            else    /* [9][5][6]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     = E_D97_RULER_QUAD_3_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                sprintf(name, "(����)����%d�������޹��ܵ���", ucDi00);         
                            }
                        }
                        break;

                        default: /* [9][5]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;
                    }
                }
                break;
            
#if 0
                case 6: /* [9][Di10 = 6][*][*] */
                {

                }
                break;

                case 7:/* [9][Di10 = 7][*][*] */
                {

                }
                break;
#endif

                case 8:/* [9]{8}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_LAST_LAST_MONTH;
                    switch(ucDi01)
                    {
                        case 1:/* [9][8]{1}[*] */
                        {
                            if(0 == ucDi00) /* [9][8][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_POWER_TOTAL;  
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;            
                                pName     =  "(������)�����й��ܵ���";                                             
                            }
                            else if(0xF == ucDi00) /* [9][8][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_POWER_BLOCK;  
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                         
                                func     =  NULL;                                 
                                pName     =  "(������)�����й��������ݿ�";            
                            }
                            else               /* [9][8][1]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_HAVE_POWER_RATE;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;            
                                sprintf(name, "(������)����%d�����й��ܵ���", ucDi00);        
                            }
                        }
                        break;

                        case 2:  /* [9][8]{2}[*] */
                        {
                            if(0 == ucDi00)/* [9][8][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_POWER_TOTAL;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                   
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;               
                                pName     =  "(������)�����й��ܵ���";                                             
                            }
                            else if(0xF == ucDi00)/* [9][8][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_POWER_BLOCK;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                    
                                pName     =  "(������)�����й��������ݿ�";            
                            }
                            else    /* [9][8][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_HAVE_POWER_RATE;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;            
                                sprintf(name, "(������)����%d�����й��ܵ���", ucDi00);        
                            }
                        }
                        break;

                        default:
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                case 9:/* [9]{9}[*][*] */
                {
                    switch(ucDi01) 
                    {
                        case 1:/* [9][9]{1}[*] */
                        {
                            if(0 == ucDi00) /*[9][9][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_POWER_TOTAL;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;              
                                pName     =  "(������)�����޹��ܵ���";                                               
                            }
                            else if(0xF == ucDi00) /*[9][9][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_POWER_BLOCK;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                    
                                pName     =  "(������)�����޹��������ݿ�";            
                            }
                            else        /*[9][9][1]{1~E} */ 
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_NONE_POWER_RATE;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;            
                                sprintf(name, "(������)����%d�����޹��ܵ���", ucDi00);       
                            }
                        }
                        break;

                        case 2:/* [9][9]{2}[*] */
                        {
                            if(0 == ucDi00)/* [9][9][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_POWER_TOTAL;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                  
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;            
                                pName     =  "(������)�����޹��ܵ���";                                            
                            }
                            else if(0xF == ucDi00)/* [9][9][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_POWER_BLOCK;    
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  UNKNOWN_LEN;                           
                                func     =  NULL;                                    
                                pName     =  "(������)�����޹��������ݿ�";           

                            }
                            else    /* [9][9][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_NONE_POWER_RATE;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                    
                                unit     =  E_D97_UNIT_KWH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                   
                                length     =  4;                                   
                                func     =  trans_d97_format_XXXXXX_XX;            
                                sprintf(name, "(������)����%d�����޹��ܵ���", ucDi00);        
                            }
                        }
                        break;
                        
                        case 3:/* [9][9]{3}[*] */
                        {
                            if(0 == ucDi00)/* [9][9][3]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(������)һ�����޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][9][3]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(������)һ�����޹��������ݿ�";                 
                            }
                            else    /* [9][9][3]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_1_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                sprintf(name, "(������)����%dһ�����޹��ܵ���", ucDi00);       
                            }
                        }
                        break;

                        case 4:/* [9][1]{4}[*] */
                        {
                            if(0 == ucDi00)/* [9][9][4]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(������)�������޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][9][4]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(������)�������޹��������ݿ�";                 
                            }
                            else    /* [9][9][4]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_4_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                sprintf(name, "(������)����%d�������޹��ܵ���", ucDi00);       
                            }
                        }
                        break;

                        case 5:/* [9][1]{5}[*] */
                        {
                            if(0 == ucDi00)/* [9][9][5]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(������)�������޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][9][5]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_POWER_BLOCK;     
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(������)�������޹��������ݿ�";                 
                            }
                            else    /* [9][9][5]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_2_NONE_POWER_RATE;      
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                sprintf(name, "(������)����%d�������޹��ܵ���", ucDi00);       
                            }
                        }
                        break;

                        case 6:/* [9][9]{6}[*] */
                        {
                            if(0 == ucDi00)/* [9][9][6]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_POWER_TOTAL;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                   
                                pName     =  "(������)�������޹��ܵ���";                                                 
                            }
                            else if(0xF == ucDi00)/* [9][9][6]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_POWER_BLOCK;        
                                format     =  E_D97_FORMAT_XXXXXX_XX;                         
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(������)�������޹��������ݿ�";                 
                            }
                            else    /* [9][9][6]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_3_NONE_POWER_RATE;         
                                format     =  E_D97_FORMAT_XXXXXX_XX;                      
                                unit     =  E_D97_UNIT_KVARH;                        
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                       
                                func     =  trans_d97_format_XXXXXX_XX;                
                                sprintf(name, "(������)����%d�������޹��ܵ���", ucDi00);       
                            }
                        }
                        break;

                        default: /* [9][1]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                default:
                    return E_D97_ERROR_ID_UNKNOWN;

            }
        } /* {9}[*][*][*] */
        break;
        
        /* ���������� */
        case 0xA: /* {A}[*][*][*] */
        {
            switch(ucDi10) 
            {
                case 0: /* [A]{0}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_CURRENT;
                    
                    switch(ucDi01)
                    {
                        case 1:/* [A][0]{1}[*] */
                        {
                            if(0 == ucDi00) /* [A][0][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  3;                                       
                                func     =  trans_d97_format_XX_XXXX;                
                                pName     =  "(��ǰ)�����й����������";                                                 
                            }
                            else if(0xF == ucDi00) /* [A][0][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  UNKNOWN_LEN;                                
                                func     =  NULL;                                     
                                pName     =  "(��ǰ)�����й�����������ݿ�";                 
                            }
                            else               /* [A][0][1]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_RATE;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                         
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  3;                                       
                                func     =  trans_d97_format_XX_XXXX;                
                                sprintf(name, "(��ǰ)����%d�����й����������", ucDi00);          
                            }
                        }
                        break;

                        case 2:  /* [A][0]{2}[*] */
                        {
                            if(0 == ucDi00)/* [A][0][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  3;                                       
                                func     =  trans_d97_format_XX_XXXX;                
                                pName     =  "(��ǰ)�����й����������";                                                 
                            }
                            else if(0xF == ucDi00)/* [A][0][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  UNKNOWN_LEN;                                
                                func     =  NULL;                                     
                                pName     =  "(��ǰ)�����й�����������ݿ�";                 

                            }
                            else    /* [A][0][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_RATE;         /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                sprintf(name, "(��ǰ)����%d�����й����������", ucDi00);         
                            }
                        }
                        break;

                        default:
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                case 1: /* [A]{1}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_CURRENT;
                    
                    switch(ucDi01) 
                    {
                        case 1:/* [A][1]{1}[*] */
                        {
                            if(0 == ucDi00) /*[9][0][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                          
                                unit     =  E_D97_UNIT_KWH;                          
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                       
                                func     =  trans_d97_format_XX_XXXX;                
                                pName     =  "(��ǰ)�����޹����������";                                                 
                            }
                            else if(0xF == ucDi00) /*[9][0][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                        
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  UNKNOWN_LEN;                                
                                func     =  NULL;                                     
                                pName     =  "(��ǰ)�����޹�����������ݿ�";                 
                            }
                            else        /*[9][0][1]{1~E} */ 
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_RATE;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func     =  trans_d97_format_XX_XXXX;                
                                sprintf(name, "(��ǰ)����%d�����޹����������", ucDi00);          
                            }
                        }
                        break;

                        case 2:/* [A][1]{2}[*] */
                        {
                            if(0 == ucDi00)/* [A][1][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func     =  trans_d97_format_XX_XXXX;                
                                pName     =  "(��ǰ)�����޹����������";                                                 
                            }
                            else if(0xF == ucDi00)/* [A][1][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  UNKNOWN_LEN;                                
                                func     =  NULL;                                     
                                pName     =  "(��ǰ)�����޹�����������ݿ�";                 
                            }
                            else    /* [A][1][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_RATE;         /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func     =  trans_d97_format_XX_XXXX;                
                                sprintf(name, "(��ǰ)����%d�����޹����������", ucDi00);          
                            }
                        }
                        break;
                        
                        case 3:/* [A][1]{3}[*] */
                        {
                            if(0 == ucDi00)/* [A][1][3]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(��ǰ)һ�����޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][1][3]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(��ǰ)һ�����޹�����������ݿ�";                 
                            }
                            else    /* [A][1][3]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                 
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(��ǰ)����%dһ�����޹����������", ucDi00);         
                            }
                        }
                        break;

                        case 4:/* [A][1]{4}[*] */
                        {
                            if(0 == ucDi00)/* [A][1][4]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(��ǰ)�������޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][1][4]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(��ǰ)�������޹�����������ݿ�";                 

                            }
                            else    /* [A][1][4]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                 
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(��ǰ)����%d�������޹����������", ucDi00);         
                            }
                        }
                        break;

                        case 5:/* [A][1]{5}[*] */
                        {
                            if(0 == ucDi00)/* [A][1][5]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(��ǰ)�������޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][1][5]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(��ǰ)�������޹�����������ݿ�";                 
                            }
                            else    /* [A][1][5]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                 
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(��ǰ)����%d�������޹����������", ucDi00);         
                            }
                        }
                        break;

                        case 6:/* [A][1]{6}[*] */
                        {
                            if(0 == ucDi00)/* [A][1][6]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(��ǰ)�������޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][1][6]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_BLOCK;         
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(��ǰ)�������޹�����������ݿ�";                 
                            }
                            else    /* [A][1][6]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                 
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(��ǰ)����%d�������޹����������", ucDi00);         
                            }

                        }
                        break;

                        default: /* [A][1]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;
#if 0
                case 2:  /* [A][Di10 = 2][*][*] */
                {

                }
                break;

                case 3: /* [A][Di10 = 3][*][*] */
                {

                }
                break;
#endif
                case 4: /* [A]{4}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_LAST_MONTH;

                    switch(ucDi01)
                    {
                        case 1:/* [A][4]{1}[*] */
                        {
                            if(0 == ucDi00) /* [A][4][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TOTAL;    /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                      
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;            
                                pName     =  "(����)�����й����������";                                             
                            }
                            else if(0xF == ucDi00) /* [A][4][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  UNKNOWN_LEN;                                
                                func     =  NULL;                                     
                                pName     =  "(����)�����й�����������ݿ�";                 

                            }
                            else               /* [A][4][1]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_RATE;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                          
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func     =  trans_d97_format_XX_XXXX;                
                                sprintf(name, "(����)����%d�����й����������", ucDi00);             
                            }
                        }
                        break;

                        case 2:  /* [A][4]{2}[*] */
                        {
                            if(0 == ucDi00)/* [A][4][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TOTAL;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                      
                                unit     =  E_D97_UNIT_KWH;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                length     =  4;                                       
                                func     =  trans_d97_format_XX_XXXX;                
                                pName     =  "(����)�����й����������";                                                 
                            }
                            else if(0xF == ucDi00)/* [A][4][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_BLOCK;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                        
                                func     =  NULL;                                 
                                pName     =  "(����)�����й�����������ݿ�";                 

                            }
                            else    /* [A][4][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_RATE;         /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                sprintf(name, "(����)����%d�����й����������" , ucDi00);         
                            }
                        }
                        break;

                        default:
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                case 5: /* [A]{5}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_LAST_MONTH;
                        
                    switch(ucDi01) 
                    {
                        case 1:/* [A][5]{1}[*] */
                        {
                            if(0 == ucDi00) /*[9][0][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                pName     =  "(����)�����޹����������";                                             
                            }
                            else if(0xF == ucDi00) /*[9][5][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                        
                                func     =  NULL;                                 
                                pName     =  "(����)�����޹�����������ݿ�";                 

                            }
                            else        /*[9][5][1]{1~E} */ 
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_RATE;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                sprintf(name, "(����)����%d�����޹����������", ucDi00);         
                            }

                        }
                        break;

                        case 2:/* [A][5]{2}[*] */
                        {
                            if(0 == ucDi00)/* [A][1][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TOTAL;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                pName     =  "(����)�����޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][5][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_BLOCK;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                        
                                func     =  NULL;                                 
                                pName     =  "(����)�����޹�����������ݿ�";                 

                            }
                            else    /* [A][5][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_RATE;         /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                sprintf(name, "(����)����%d�����޹����������", ucDi00);         
                            }

                        }
                        break;
                        
                        case 3:/* [A][5]{3}[*] */
                        {
                            if(0 == ucDi00)/* [A][5][3]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(����)һ�����޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][5][3]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(����)һ�����޹�����������ݿ�";                 

                            }
                            else    /* [A][5][3]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                 
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(����)����%dһ�����޹����������", ucDi00);         
                            }

                        }
                        break;

                        case 4:/* [A][5]{4}[*] */
                        {
                            if(0 == ucDi00)/* [A][5][4]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(����)�������޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][5][4]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(����)�������޹�����������ݿ�";                 

                            }
                            else    /* [A][5][4]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                 
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(����)����%d�������޹����������", ucDi00);         
                            }

                        }
                        break;

                        case 5:/* [A][5]{5}[*] */
                        {
                            if(0 == ucDi00)/* [A][5][5]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(����)�������޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][5][5]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(����)�������޹�����������ݿ�";                 

                            }
                            else    /* [A][5][5]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                 
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(����)����%d�������޹����������", ucDi00);         
                            }

                        }
                        break;

                        case 6:/* [A][5]{6}[*] */
                        {
                            if(0 == ucDi00)/* [A][5][6]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(����)�������޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][5][6]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                       
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(����)�������޹�����������ݿ�";        

                            }
                            else    /* [A][5][6]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                    
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(����)����%d�������޹����������", ucDi00); 
                            }

                        }
                        break;

                        default: /* [A][5]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;
#if 0
                case 6: /* [A][Di10 = 6][*][*] */
                {

                }
                break;

                case 7:/* [A][Di10 = 7][*][*] */
                {

                }
                break;
#endif
                case 8:/* [A]{8}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_LAST_LAST_MONTH;
                    switch(ucDi01)
                    {
                        case 1:/* [A][8]{1}[*] */
                        {
                            if(0 == ucDi00) /* [A][8][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                pName     =  "(������)�����й����������";                                              
                            }
                            else if(0xF == ucDi00) /* [A][8][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                        
                                func     =  NULL;                                 
                                pName     =  "(������)�����й�����������ݿ�";      

                            }
                            else               /* [A][8][1]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_RATE;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                sprintf(name, "(������)����%d�����й����������", ucDi00);  
                            }
                        }
                        break;

                        case 2:  /* [A][8]{2}[*] */
                        {
                            if(0 == ucDi00)/* [A][8][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TOTAL;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                pName     =  "(������)�����й����������"  ;                                             
                            }
                            else if(0xF == ucDi00)/* [A][8][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_BLOCK;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                        
                                func     =  NULL;                                 
                                pName     =  "(������)�����й�����������ݿ�";                 

                            }
                            else    /* [A][8][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_RATE;         /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                sprintf(name, "(������)����%d�����й����������", ucDi00);         
                            }
                        }
                        break;

                        default:
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                case 0x9:/* [A]{A}[*][*] */
                {
                    switch(ucDi01) 
                    {
                        case 1:/* [A][9]{1}[*] */
                        {
                            if(0 == ucDi00) /*[9][9][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                pName     =  "(������)�����޹����������";                                             
                            }
                            else if(0xF == ucDi00) /*[9][9][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                        
                                func     =  NULL;                                 
                                pName     =  "(������)�����޹�����������ݿ�";                 

                            }
                            else        /*[9][9][1]{1~E} */ 
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_RATE;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                sprintf(name, "(������)����%d�����޹����������", ucDi00);         
                            }

                        }
                        break;

                        case 2:/* [A][9]{2}[*] */
                        {
                            if(0 == ucDi00)/* [A][9][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TOTAL;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                pName     =  "(������)�����޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][9][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_BLOCK;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  UNKNOWN_LEN;                        
                                func     =  NULL;                                 
                                pName     =  "(������)�����޹�����������ݿ�";                 

                            }
                            else    /* [A][9][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_RATE;         /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_XX_XXXX;                  
                                unit     =  E_D97_UNIT_KWH;                     
                                rdwr     =  E_D97_RDWR_READ_ONLY;                
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;        
                                sprintf(name, "(������)����%d�����޹����������", ucDi00);         
                            }

                        }
                        break;
                        
                        case 3:/* [A][9]{3}[*] */
                        {
                            if(0 == ucDi00)/* [A][9][3]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(������)һ�����޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][9][3]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(������)һ�����޹�����������ݿ�";                 

                            }
                            else    /* [A][9][3]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                 
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(������)����%dһ�����޹����������", ucDi00);         
                            }

                        }
                        break;

                        case 4:/* [A][1]{4}[*] */
                        {
                            if(0 == ucDi00)/* [A][9][4]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(������)�������޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][9][4]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(������)�������޹�����������ݿ�";                 

                            }
                            else    /* [A][9][4]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                 
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(������)����%d�������޹����������", ucDi00);         
                            }

                        }
                        break;

                        case 5:/* [A][1]{5}[*] */
                        {
                            if(0 == ucDi00)/* [A][9][5]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TOTAL;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(������)�������޹����������";                                             
                            }
                            else if(0xF == ucDi00)/* [A][9][5]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                                
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(������)�������޹�����������ݿ�";                 

                            }
                            else    /* [A][9][5]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                    
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(������)����%d�������޹����������", ucDi00);         
                            }

                        }
                        break;

                        case 6:/* [A][9]{6}[*] */
                        {
                            if(0 == ucDi00)/* [A][9][6]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TOTAL;/* ��Լ����  */ 
                                format     =  E_D97_FORMAT_XX_XXXX;                          
                                unit     =  E_D97_UNIT_KVARH;                               
                                rdwr     =  E_D97_RDWR_READ_ONLY;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_XX_XXXX;                        
                                pName     =  "(������)�������޹����������";                                      
                            }
                            else if(0xF == ucDi00)/* [A][9][6]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_BLOCK;        
                                format     =  E_D97_FORMAT_XX_XXXX;                       
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  UNKNOWN_LEN;                          
                                func     =  NULL;                                   
                                pName     =  "(������)�������޹�����������ݿ�";      

                            }
                            else    /* [A][9][6]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_RATE;         
                                format     =  E_D97_FORMAT_XX_XXXX;                    
                                unit     =  E_D97_UNIT_KVARH;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                      
                                length     =  4;                                   
                                func     =  trans_d97_format_XX_XXXX;          
                                sprintf(name, "(������)����%d�������޹����������", ucDi00);         
                            }

                        }
                        break;

                        default: /* [A][1]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                default:
                    return E_D97_ERROR_ID_UNKNOWN;

            }
        } /* {A}[*][*][*] */
        break;

        /* �����������ʱ�� */
        case 0xB: /* {B}[*][*][*] */
        {
            switch(ucDi10) 
            {
                case 0: /* [B]{0}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_CURRENT;
                    
                    switch(ucDi01)
                    {
                        case 1:/* [B][0]{1}[*] */
                        {
                            if(0 == ucDi00) /* [B][0][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TIME_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(��ǰ)�����й��������������ʱ��";                                     
                            }
                            else if(0xF == ucDi00) /* [B][0][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TIME_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(��ǰ)�����й������������ʱ�����ݿ�"  ;                 

                            }
                            else               /* [B][0][1]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TIME_RATE;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(��ǰ)����%d�����й��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        case 2:  /* [B][0]{2}[*] */
                        {
                            if(0 == ucDi00)/* [B][0][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TIME_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(��ǰ)�����й��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][0][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TIME_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(��ǰ)�����й������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][0][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TIME_RATE;         /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(��ǰ)����%d�����й��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        default:
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                case 1: /* [B]{1}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_CURRENT;
                    
                    switch(ucDi01) 
                    {
                        case 1:/* [B][1]{1}[*] */
                        {
                            if(0 == ucDi00) /*[B][0][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TIME_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(��ǰ)�����޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00) /*[B][0][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TIME_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(��ǰ)�����޹������������ʱ�����ݿ�";                 

                            }
                            else        /*[B][0][1]{1~E} */ 
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TIME_RATE;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(��ǰ)����%d�����޹��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        case 2:/* [B][1]{2}[*] */
                        {
                            if(0 == ucDi00)/* [B][1][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TIME_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(��ǰ)�����޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][1][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TIME_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(��ǰ)�����޹������������ʱ�����ݿ�";                 
                            }
                            else    /* [B][1][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TIME_RATE;         /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(��ǰ)����%d�����޹��������������ʱ��", ucDi00);         
                            }
                        }
                        break;
                        
                        case 3:/* [B][1]{3}[*] */
                        {
                            if(0 == ucDi00)/* [B][1][3]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(��ǰ)һ�����޹��������������ʱ��";                                                  
                            }
                            else if(0xF == ucDi00)/* [B][1][3]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(��ǰ)һ�����޹������������ʱ�����ݿ�";                 
                            }
                            else    /* [B][1][3]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(��ǰ)����%dһ�����޹��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        case 4:/* [B][1]{4}[*] */
                        {
                            if(0 == ucDi00)/* [B][1][4]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(��ǰ)�������޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][1][4]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(��ǰ)�������޹������������ʱ�����ݿ�";                 
                            }
                            else    /* [B][1][4]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(��ǰ)����%d�������޹��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        case 5:/* [B][1]{5}[*] */
                        {
                            if(0 == ucDi00)/* [B][1][5]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(��ǰ)�������޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][1][5]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(��ǰ)�������޹������������ʱ�����ݿ�";                 
                            }
                            else    /* [B][1][5]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(��ǰ)����%d�������޹��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        case 6:/* [B][1]{6}[*] */
                        {
                            if(0 == ucDi00)/* [B][1][6]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(��ǰ)�������޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][1][6]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(��ǰ)�������޹������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][1][6]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(��ǰ)����%d�������޹��������������ʱ��", ucDi00);         
                            }

                        }
                        break;

                        default: /* [B][1]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;


                /* ��Ӧ�ڱ�A4 �������ݱ���� */
                case 2:  /* [B]{2}[*][*] */
                {
                    switch(ucDi01)
                    {
                        case 1: /* [B][2]{1}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0:/* [B][2][1]{0} */
                                {
                                    type     =  E_D97_RULER_RECENT_PROGRAME_TIME;              /* ��Լ���� */
                                    format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                           
                                    func     =  trans_d97_format_MMDDHHmm;                    
                                    pName     =  "���һ�α��ʱ��";                            
                                }
                                break;
                            
                                case 1:/* [B][2][1]{1} */
                                {                                    
                                    type     =  E_D97_RULER_RECENT_CLEAR_DEMAND_TIME;           /* ��Լ���� */
                                     format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                           
                                    func     =  trans_d97_format_MMDDHHmm;                    
                                    pName     =  "���һ�����������0ʱ��";                     
                                }
                                break;
                                
                                case 2:/* [B][2][1]{2} */
                                {                                    
                                    type     =  E_D97_RULER_PROGRAME_TIMES;              /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNN;                          
                                    unit     =  E_D97_UNIT_TIME;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  2;                                       
                                    func     =  trans_d97_format_NNNN;                       
                                    pName     =  "��̴���";                                
                                }
                                break;
                                
                                case 3:/* [B][2][1]{3} */
                                {
                                    type     =  E_D97_RULER_CLEAR_DEMAND_TIMES;          /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNN;                      
                                    unit     =  E_D97_UNIT_TIME;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  2;                                       
                                    func     =  trans_d97_format_NNNN;                       
                                    pName     =  "���������0����";                        
                                }
                                break;
                                
                                case 4:/* [B][2][1]{4} */
                                {
                                    type     =  E_D97_RULER_CLEAR_DEMAND_TIMES;          /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNNNN;                      
                                    unit     =  E_D97_UNIT_mm;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  3;                                       
                                    func     =  trans_d97_format_NNNNNN;                       
                                    pName     =  "��ع���ʱ��";                        
                                }
                                break;
                                
                                default: /* [B][2]{1}[*] */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            
                            }
                        }
                        break;

                        default: /* [B][2]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;
                            
                    }

                }
                break;


                /* ��Ӧ�ڱ�A4 �������ݱ���� */
                case 3: /* [B]{3}[*][*] */
                {
                    switch(ucDi01)
                    {
                        case 1:/* [B][3]{1}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [B][3][1]{0} */
                                {
                                    
                                    type     =  E_D97_RULER_PHASE_LOSS_TOTAL_TIMES;     /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNN;                      
                                    unit     =  E_D97_UNIT_TIME;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  2;                                       
                                    func     =  trans_d97_format_NNNN;                       
                                    pName     =  "�ܶ������";                            
                                }
                                break;
                                
                                case 1: /* [B][3][1]{1} */
                                {
                                    
                                    type     =  E_D97_RULER_PHASE_A_LOSS_TIMES;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNN;                      
                                    unit     =  E_D97_UNIT_TIME;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  2;                                       
                                    func     =  trans_d97_format_NNNN;                       
                                    pName     =  "A��������";                            
                                }
                                break;
                                
                                case 2: /* [B][3][1]{2} */
                                {
                                    type     =  E_D97_RULER_PHASE_B_LOSS_TIMES;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNN;                      
                                    unit     =  E_D97_UNIT_TIME;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  2;                                       
                                    func     =  trans_d97_format_NNNN;                       
                                    pName     =  "B��������";                             
                                }
                                break;
                                
                                case 3: /* [B][3][1]{3} */
                                {
                                    
                                    type     =  E_D97_RULER_PHASE_C_LOSS_TIMES;             /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNN;                      
                                    unit     =  E_D97_UNIT_TIME;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  2;                                       
                                    func     =  trans_d97_format_NNNN;                       
                                    pName     =  "C��������";                             
                                }
                                break;

                                default: /* [B][3][1]{*} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                                                                        
                            }
                        }
                        break;

                        case 2:/* [B][3]{2}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [B][3][2]{0} */
                                {
                                    type     =  E_D97_RULER_PHASE_LOSS_SPEND;             /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNNNN;                      
                                    unit     =  E_D97_UNIT_mm;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  3;                                       
                                    func     =  trans_d97_format_NNNNNN;                       
                                    pName     =  "����ʱ���ۻ�ֵ";                        
                                }
                                break;
                                
                                case 1: /* [B][3][2]{1} */
                                {
                                    
                                        type     =  E_D97_RULER_PHASE_A_LOSS_SPEND;      /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNNNN;                      
                                    unit     =  E_D97_UNIT_mm;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  3;                                       
                                    func     =  trans_d97_format_NNNNNN;                       
                                    pName     =  "A�����ʱ���ۻ�ֵ";                        
                                }
                                break;
                                
                                case 2: /* [B][3][2]{2} */
                                {
                                    
                                        type     =  E_D97_RULER_PHASE_B_LOSS_SPEND;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNNNN;                      
                                    unit     =  E_D97_UNIT_mm;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  3;                                       
                                    func     =  trans_d97_format_NNNNNN;                       
                                    pName     =  "B�����ʱ���ۻ�ֵ";                        
                                }
                                break;
                                
                                case 3: /* [B][3][2]{3} */
                                {
                                    
                                        type     =  E_D97_RULER_PHASE_C_LOSS_SPEND;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNNNN;                      
                                    unit     =  E_D97_UNIT_mm;                           
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  3;                                       
                                    func     =  trans_d97_format_NNNNNN;                       
                                    pName     =  "C�����ʱ���ۻ�ֵ";                        
                                }
                                break;

                                default: /* [B][3][2]{*} */
                                        return E_D97_ERROR_ID_UNKNOWN;
                            }
                        }
                        break;

                        case 3:/* [B][3]{3}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [B][3][3]{0} */
                                {                                    
                                    type     =  E_D97_RULER_RECENT_PHASE_LOSS_START_TIME;       /* ��Լ���� */
                                    format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                       
                                    func     =  trans_d97_format_MMDDHHmm;            
                                    pName     =  "���һ�ζ�����ʼʱ��";                 
                                }
                                break;
                                
                                case 1: /* [B][3][3]{1} */
                                {                                    
                                    type     =  E_D97_RULER_RECENT_PHASE_A_LOSS_START_TIME;   /* ��Լ���� */
                                    format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                       
                                    func     =  trans_d97_format_MMDDHHmm;            
                                    pName     =  "A�����������ʼʱ��";                    
                                }
                                break;
                                
                                case 2: /* [B][3][3]{2} */
                                {                                    
                                    type     =  E_D97_RULER_RECENT_PHASE_B_LOSS_START_TIME;   /* ��Լ���� */
                                    format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                       
                                    func     =  trans_d97_format_MMDDHHmm;            
                                    pName     =  "B�����������ʼʱ��";                    
                                }
                                break;
                                
                                case 3: /* [B][3][3]{3} */
                                {                                    
                                    type     =  E_D97_RULER_RECENT_PHASE_A_LOSS_START_TIME;   /* ��Լ���� */
                                    format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                       
                                    func     =  trans_d97_format_MMDDHHmm;            
                                    pName     =  "C�����������ʼʱ��";                    
                                }
                                break;

                                default: /* [B][3][3]{*} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }

                        }
                        break;

                        case 4:/* [B][3]{4}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [B][3][4]{0} */
                                {                                    
                                    type     =  E_D97_RULER_RECENT_PHASE_LOSS_END_TIME;       /* ��Լ���� */
                                    format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                       
                                    func     =  trans_d97_format_MMDDHHmm;            
                                    pName     =  "���һ�ζ������ʱ��";                 
                                }
                                break;
                                
                                case 1: /* [B][3][4]{1} */
                                {                                    
                                    type     =  E_D97_RULER_RECENT_PHASE_A_LOSS_END_TIME;   /* ��Լ���� */
                                    format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                       
                                    func     =  trans_d97_format_MMDDHHmm;            
                                    pName     =  "A������������ʱ��";                    
                                }
                                break;
                                
                                case 2: /* [B][3][4]{2} */
                                {                                    
                                    type     =  E_D97_RULER_RECENT_PHASE_B_LOSS_END_TIME;   /* ��Լ���� */
                                    format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                       
                                    func     =  trans_d97_format_MMDDHHmm;            
                                    pName     =  "B������������ʱ��";                    
                                }
                                break;
                                
                                case 3: /* [B][3][4]{3} */
                                {                                    
                                    type     =  E_D97_RULER_RECENT_PHASE_C_LOSS_END_TIME;   /* ��Լ���� */
                                    format     =  E_D97_FORMAT_MMDDHHmm;                          
                                    unit     =  E_D97_UNIT_MMDDHHmm;                            
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                        
                                    length     =  4;                                       
                                    func     =  trans_d97_format_MMDDHHmm;            
                                    pName     =  "C������������ʱ��";                    
                                }
                                break;

                                default: /* [B][3][4]{*} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }

                        }
                        break;
                        
                        default: /* [B][3]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;


                /* ���������������ʱ����� */
                case 4: /* [B]{4}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_LAST_MONTH;

                    switch(ucDi01)
                    {
                        case 1:/* [B][4]{1}[*] */
                        {
                            if(0 == ucDi00) /* [B][4][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TIME_TOTAL;    /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                         
                                length     =  4;                                           
                                func     =  trans_d97_format_MMDDHHmm;                     
                                pName     =  "(����)�����й��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00) /* [B][4][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TIME_BLOCK;    /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                         
                                length     =  UNKNOWN_LEN;                                 
                                func     =  NULL;                                          
                                pName     =  "(����)�����й������������ʱ�����ݿ�";             

                            }
                            else               /* [B][4][1]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TIME_RATE;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                         
                                rdwr     =  E_D97_RDWR_READ_ONLY;                         
                                length     =  4;                                           
                                func     =  trans_d97_format_MMDDHHmm;                     
                                sprintf(name, "(����)����%d�����й��������������ʱ��", ucDi00);
                                pName     =  name;      
                            }
                        }
                        break;

                        case 2:  /* [B][4]{2}[*] */
                        {
                            if(0 == ucDi00)/* [B][4][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TIME_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(����)�����й��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][4][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TIME_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(����)�����й������������ʱ�����ݿ�";                 
                            }
                            else    /* [B][4][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TIME_RATE;         /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(����)����%d�����й��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        default:
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                case 5: /* [B]{5}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_LAST_MONTH;
                        
                    switch(ucDi01) 
                    {
                        case 1:/* [B][5]{1}[*] */
                        {
                            if(0 == ucDi00) /*[B][0][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TIME_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(����)�����޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00) /*[B][5][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TIME_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(����)�����޹������������ʱ�����ݿ�";                 

                            }
                            else        /*[B][5][1]{1~E} */ 
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TIME_RATE;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(����)����%d�����޹��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        case 2:/* [B][5]{2}[*] */
                        {
                            if(0 == ucDi00)/* [B][1][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TIME_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(����)�����޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][5][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TIME_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(����)�����޹������������ʱ�����ݿ�";                 
                            }
                            else    /* [B][5][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TIME_RATE;         /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(����)����%d�����޹��������������ʱ��", ucDi00);         
                            }
                        }
                        break;
                        
                        case 3:/* [B][5]{3}[*] */
                        {
                            if(0 == ucDi00)/* [B][5][3]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(����)һ�����޹��������������ʱ��";                                                  
                            }
                            else if(0xF == ucDi00)/* [B][5][3]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(����)һ�����޹������������ʱ�����ݿ�";                 
                            }
                            else    /* [B][5][3]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(����)����%dһ�����޹��������������ʱ��", ucDi00);         
                            }

                        }
                        break;

                        case 4:/* [B][5]{4}[*] */
                        {
                            if(0 == ucDi00)/* [B][5][4]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                             
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                               
                                func     =  trans_d97_format_MMDDHHmm;                             
                                pName     =  "(����)�������޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][5][4]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                               
                                length     =  UNKNOWN_LEN;                                     
                                func     =  NULL;                                                
                                pName     =  "(����)�������޹������������ʱ�����ݿ�";               

                            }
                            else    /* [B][5][4]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_RATE;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                 
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                               
                                length     =  4;                                                  
                                func     =  trans_d97_format_MMDDHHmm;                           
                                sprintf(name, "(����)����%d�������޹��������������ʱ��", ucDi00);     
                            }

                        }
                        break;

                        case 5:/* [B][5]{5}[*] */
                        {
                            if(0 == ucDi00)/* [B][5][5]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(����)�������޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][5][5]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(����)�������޹������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][5][5]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(����)����%d�������޹��������������ʱ��", ucDi00);         
                            }

                        }
                        break;

                        case 6:/* [B][5]{6}[*] */
                        {
                            if(0 == ucDi00)/* [B][5][6]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(����)�������޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][5][6]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(����)�������޹������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][5][6]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(����)����%d�������޹��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        default: /* [B][5]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;
                    }
                }
                break;

                case 6: /* [B]{6}[*][*] */
                {
                    switch(ucDi01)
                    {
                        case 1:/* [B][6]{1}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 1: /* [B][6][1]{1} */
                                {
                                    type     =  E_D97_RULER_PHASE_A_VOLT;          
                                    format     =  E_D97_FORMAT_XXX;          
                                    unit     =  E_D97_UNIT_V;             
                                    rdwr     =  E_D97_RDWR_READ_ONLY;      
                                    length     = 2;                         
                                    func     =  trans_d97_format_XXX; 
                                    pName     =  "A���ѹ";                
                                }
                                break;

                                case 2: /* [B][6][1]{2} */
                                {
                                    type     =  E_D97_RULER_PHASE_B_VOLT;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XXX;          
                                    unit     =  E_D97_UNIT_V;             
                                    rdwr     =  E_D97_RDWR_READ_ONLY;      
                                    length     = 2;                         
                                    func     =  trans_d97_format_XXX; 
                                    pName     =  "B���ѹ";                
                                }
                                break;
                                
                                case 3: /* [B][6][1]{3} */
                                {
                                    
                                        type     =  E_D97_RULER_PHASE_C_VOLT;         /* ��Լ���� */
                                        
                                    format     =  E_D97_FORMAT_XXX;          
                                    unit     =  E_D97_UNIT_V;             
                                    rdwr     =  E_D97_RDWR_READ_ONLY;      
                                    length     = 2;                         
                                    func     =  trans_d97_format_XXX; 
                                    pName     =  "C���ѹ";                
                                }
                                break;

                                default: /* [B][6][1]{*} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                                                                        
                            }
                        }
                        break;

                        case 2:/* [B][6]{2}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 1: /* [B][6][2]{1} */
                                    {                                
                                        type     =  E_D97_RULER_PHASE_A_ELEC;               
                                        format     =  E_D97_FORMAT_XX_XX;        
                                        unit     =  E_D97_UNIT_A;              
                                        rdwr     =  E_D97_RDWR_READ_ONLY;      
                                        length     = 2;                         
                                        func     =  trans_d97_format_XX_XX; 
                                        pName     =  "A�����";                
                                    }
                                    break;

                                    case 2: /* [B][6][2]{2} */
                                    {                                        
                                        type     =  E_D97_RULER_PHASE_B_ELEC;         /* ��Լ���� */
                                        format     =  E_D97_FORMAT_XX_XX;        
                                        unit     =  E_D97_UNIT_A;              
                                        rdwr     =  E_D97_RDWR_READ_ONLY;      
                                        length     = 2;                         
                                        func     =  trans_d97_format_XX_XX; 
                                        pName     =  "B�����";                
                                    }
                                    break;
                                    case 3: /* [B][6][2]{3} */
                                    {
                                        type     =  E_D97_RULER_PHASE_C_ELEC;         /* ��Լ���� */
                                        format     =  E_D97_FORMAT_XX_XX;        
                                        unit     =  E_D97_UNIT_A;              
                                        rdwr     =  E_D97_RDWR_READ_ONLY;      
                                        length     = 2;                         
                                        func     =  trans_d97_format_XX_XX; 
                                        pName     =  "C�����";                
                                    }
                                    break;

                                    default: /* [B][6][2]{*} */
                                        return E_D97_ERROR_ID_UNKNOWN;
                            }
                        }
                        break;

                        case 3:/* [B][6]{3}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [B][6][3]{0} */
                                {
                                    type     =  E_D97_RULER_INSTANT_HAVE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX_XXXX;                  
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     = 3;                                       
                                    func     =  trans_d97_format_XX_XXXX;           
                                    pName     =  "˲ʱ�й�����";                          
                                }
                                break;
                                
                                case 1: /* [B][6][3]{1} */
                                {                                    
                                    type     =  E_D97_RULER_PHASE_A_HAVE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX_XXXX;                  
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  3;                                       
                                    func     =  trans_d97_format_XX_XXXX;           
                                    pName     =  "A���й�����";                          
                                }
                                break;
                                
                                case 2: /* [B][6][3]{2} */
                                {                                    
                                    type     =  E_D97_RULER_PHASE_B_HAVE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX_XXXX;                  
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  3;                                       
                                    func     =  trans_d97_format_XX_XXXX;           
                                    pName     =  "B���й�����";                          
                                }
                                break;
                                
                                case 3: /* [B][6][3]{3} */
                                {                                    
                                    type     =  E_D97_RULER_PHASE_C_HAVE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX_XXXX;                  
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  3;                                       
                                    func     =  trans_d97_format_XX_XXXX;           
                                    pName     =  "C���й�����";                          
                                }
                                break;

                                case 4: /* [B][6][3]{4} */
                                {                                    
                                    type     =  E_D97_RULER_FORTH_HAVE_POWER_RATE_LIMIT;         /* ��Լ���� */                                         
                                    format     =  E_D97_FORMAT_XX_XX;                        
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_WRITE;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_XX_XX;                 
                                    pName     =  "�����й���������ֵ";                   
                                }
                                break;

                                case 5: /* [B][6][3]{5} */
                                {                                    
                                    type     =  E_D97_RULER_BACK_HAVE_POWER_RATE_LIMIT;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX_XX;                        
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_WRITE;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_XX_XX;                 
                                    pName     =  "�����й���������ֵ";                     
                                }
                                break;
                                
                                default: /* [B][6][3]{*} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }
                        }
                        break;

                        case 4:/* [B][6]{4}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [B][6][4]{0} */
                                {                                    
                                    type     =  E_D97_RULER_INSTANT_NONE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX_XX;                        
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_XX_XX;                 
                                    pName     =  "˲ʱ�޹�����";                          
                                }
                                break;
                                
                                case 1: /* [B][6][4]{1} */
                                {                                    
                                    type     =  E_D97_RULER_PHASE_A_NONE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX_XX;                  
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_XX_XX;           
                                    pName     =  "A���޹�����";                          
                                }
                                break;
                                
                                case 2: /* [B][6][4]{2} */
                                {                                    
                                    type     =  E_D97_RULER_PHASE_A_NONE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX_XX;                  
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_XX_XX;           
                                    pName     =  "B���޹�����";                          
                                }
                                break;
                                
                                case 3: /* [B][6][4]{3} */
                                {                                    
                                    type     =  E_D97_RULER_PHASE_A_NONE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX_XX;                  
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_XX_XX;           
                                    pName     =  "C���޹�����";                          
                                }
                                break;

                                default: /* [B][6][4]{*} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }
                        }
                        break;
                        
                        case 5:/* [B][6]{5}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [B][6][5]{0} */
                                {                                    
                                    type     =  E_D97_RULER_INSTANT_NONE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_X_XXX;                       
                                    unit     =  E_D97_UNIT_FACTOR;                    
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_X_XXX;                
                                    pName     =  "�ܹ�������";                                
                                }
                                break;
                                
                                case 1: /* [B][6][5]{1} */
                                {                                    
                                    type     =  E_D97_RULER_PHASE_A_NONE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_X_XXX;                       
                                    unit     =  E_D97_UNIT_FACTOR;                    
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_X_XXX;               
                                    pName     =  "A�๦������";                          
                                }
                                break;
                                
                                case 2: /* [B][6][5]{2} */
                                {                                    
                                    type     =  E_D97_RULER_PHASE_A_NONE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_X_XXX;                        
                                    unit     =  E_D97_UNIT_FACTOR;                    
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_X_XXX;               
                                    pName     =  "B�๦������";                          
                                }
                                break;
                                
                                case 3: /* [B][6][5]{3} */
                                {
                                    
                                    type     =  E_D97_RULER_PHASE_A_NONE_POWER_RATE;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_X_XXX;                       
                                    unit     =  E_D97_UNIT_KW;                        
                                    rdwr     =  E_D97_RDWR_READ_ONLY;                    
                                    length     =  2;                             
                                    func     =  trans_d97_format_X_XXX;                
                                    pName     =  "C�๦������";                          
                                }
                                break;

                                default: /* [B][6][5]{*} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }
                        }
                        break;
                        
                        default: /* [B][6]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }

                }
                break;

#if 0                
                case 7:/* [B][Di10 = 7][*][*] */
                {

                }
                break;
#endif
                case 8:/* [B]{8}[*][*] */
                {
                    para.checkPoint = E_D97_CHECK_POINT_LAST_LAST_MONTH;
                    switch(ucDi01)
                    {
                        case 1:/* [B][8]{1}[*] */
                        {
                            if(0 == ucDi00) /* [B][8][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TIME_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(������)�����й��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00) /* [B][8][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TIME_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(������)�����й������������ʱ�����ݿ�";                 

                            }
                            else               /* [B][8][1]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_HAVE_DEMAND_TIME_RATE;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(������)����%d�����й��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        case 2:  /* [B][8]{2}[*] */
                        {
                            if(0 == ucDi00)/* [B][8][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TIME_TOTAL;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(������)�����й��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][8][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TIME_BLOCK;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(������)�����й������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][8][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_HAVE_DEMAND_TIME_RATE;         /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(������)����%d�����й��������������ʱ��", ucDi00);         
                            }
                        }
                        break;

                        default:
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                case 9:/* [B]{A}[*][*] */
                {
                    switch(ucDi01) 
                    {
                        case 1:/* [B][9]{1}[*] */
                        {
                            if(0 == ucDi00) /*[B][9][1]{0} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TIME_TOTAL;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(������)�����޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00) /*[B][9][1]{F} */
                            {
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TIME_BLOCK;        /* ��Լ���� */
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(������)�����޹������������ʱ�����ݿ�";                 

                            }
                            else        /*[B][9][1]{1~E} */ 
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_FORTH_NONE_DEMAND_TIME_RATE;    
                                format     =  E_D97_FORMAT_MMDDHHmm;                        
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                         
                                length     =  4;                                           
                                func     =  trans_d97_format_MMDDHHmm;                 
                                sprintf(name, "(������)����%d�����޹��������������ʱ��", ucDi00); 
                            }
                        }
                        break;

                        case 2:/* [B][9]{2}[*] */
                        {
                            if(0 == ucDi00)/* [B][9][2]{0} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TIME_TOTAL;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                pName     =  "(������)�����޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][9][2]{F} */
                            {
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TIME_BLOCK;        /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                      
                                pName     =  "(������)�����޹������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][9][2]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_BACK_NONE_DEMAND_TIME_RATE;         /* ��Լ���� */                                
                                format     =  E_D97_FORMAT_MMDDHHmm;                           
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                     
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;             
                                sprintf(name, "(������)����%d�����޹��������������ʱ��", ucDi00);         
                            }

                        }
                        break;
                        
                        case 3:/* [B][9]{3}[*] */
                        {
                            if(0 == ucDi00)/* [B][9][3]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(������)һ�����޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][9][3]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(������)һ�����޹������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][9][3]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(������)����%dһ�����޹��������������ʱ��", ucDi00);         
                            }

                        }
                        break;

                        case 4:/* [B][1]{4}[*] */
                        {
                            if(0 == ucDi00)/* [B][9][4]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(������)�������޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][9][4]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(������)�������޹������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][9][4]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(������)����%d�������޹��������������ʱ��", ucDi00);         
                            }

                        }
                        break;

                        case 5:/* [B][1]{5}[*] */
                        {
                            if(0 == ucDi00)/* [B][9][5]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(������)�������޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][9][5]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(������)�������޹������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][9][5]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(������)����%d�������޹��������������ʱ��", ucDi00);         
                            }

                        }
                        break;

                        case 6:/* [B][9]{6}[*] */
                        {
                            if(0 == ucDi00)/* [B][9][6]{0} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_TOTAL;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                            
                                rdwr     =  E_D97_RDWR_READ_ONLY;                                 
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;                 
                                pName     =  "(������)�������޹��������������ʱ��";                                             
                            }
                            else if(0xF == ucDi00)/* [B][9][6]{F} */
                            {
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_BLOCK;        
                                format     =  E_D97_FORMAT_MMDDHHmm;                                    
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  UNKNOWN_LEN;                             
                                func     =  NULL;                                        
                                pName     =  "(������)�������޹������������ʱ�����ݿ�";                 

                            }
                            else    /* [B][9][6]{1~E} */
                            {
                                para.rate_k = ucDi00;
                                type     =  E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_RATE;         
                                format     =  E_D97_FORMAT_MMDDHHmm;                     
                                unit     =  E_D97_UNIT_MMDDHHmm;                              
                                rdwr     =  E_D97_RDWR_READ_ONLY;                       
                                length     =  4;                                   
                                func     =  trans_d97_format_MMDDHHmm;               
                                sprintf(name, "(������)����%d�������޹��������������ʱ��", ucDi00);         
                            }

                        }
                        break;

                        default: /* [B][1]{*}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                default:
                    return E_D97_ERROR_ID_UNKNOWN;

            }
        } /* {B}[*][*][*] */
        break;

       /* �α������ݱ�ʶ����� ��Ӧ�ڱ�A5 */
       case 0xC: /* {C}[*][*][*] */
           {
            switch(ucDi10)
            {
                case 0: /* [C]{0}[*][*] */
                {
                    switch(ucDi01)
                    {
                        case 1: /* [C][0]{1}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [C][0][1]{0} */
                                {                                    
                                    type     =  E_D97_RULER_DATE_AND_WEEK_NUMBER;         /* ��Լ���� */
                                    format     =  E_D97_FORMAT_YYMMDDWW;             
                                    unit     =  E_D97_UNIT_YYMMDDWW;             
                                    rdwr     = E_D97_RDWR_READ_WRITE;               
                                    length     =  4;                                   
                                    func     =  trans_d97_format_YYMMDDWW;    
                                    pName     =  "���ڼ��ܴ�";                        
                                    
                                }
                                break;
                                
                                case 1: /* [C][0][1]{1} */
                                {                                    
                                    type     =  E_D97_RULER_TIME;                /* ��Լ���� */
                                    format     =  E_D97_FORMAT_hhmmss;             
                                    unit     =  E_D97_UNIT_hhmmss;             
                                    rdwr     =  E_D97_RDWR_READ_WRITE;           
                                    length     =  3;                             
                                    func     =  trans_d97_format_hhmmss;    
                                    pName     =  "ʱ��";                        
                                }
                                break;
                            }
                        }
                        break;

                        case 2: /* [C][0]{2}[*] ״̬�� */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [C][0][2]{0} */
                                {                                    
                                    type     =  E_D97_RULER_STWD_METER_RUN;    /* ��Լ���� */
                                    format     =  E_D97_FORMAT_STWD_METER_RUN;   
                                    unit     =  E_D97_UNIT_stwd;                 
                                    rdwr     =  E_D97_RDWR_READ_WRITE;           
                                    length     =  1;                             
                                    func     =  trans_d97_format_stwd_meter;      
                                    pName     =  "�������״̬��";             
                                }
                                break;
                                
                                case 1: /* [C][0][2]{1} */
                                {                                    
                                    type     =  E_D97_RULER_STWD_NETWIRE_RUN;    /* ��Լ���� */
                                    format     =  E_D97_FORMAT_STWD_NETWIRE_RUN;   
                                    unit     =  E_D97_UNIT_stwd;                    
                                    rdwr     = E_D97_RDWR_READ_WRITE;             
                                    length     = 1;                             
                                    func     =  trans_d97_format_stwd_netwire;
                                    pName     =  "����״̬��";                    
                                }
                                break;

                                case 2: /* [C][0][2]{2} */
                                {                                    
                                    type     =  E_D97_RULER_STWD_WEEK_DAY;             
                                    format     =  E_D97_FORMAT_STWD_WEEK_DAY;           
                                    unit     =  E_D97_UNIT_stwd;                        
                                    rdwr     = E_D97_RDWR_READ_WRITE;                   
                                    length     = 1;                                     
                                    func     =  trans_d97_format_stwd_weekday;   
                                    pName     =  "������״̬��";                     
                                }
                                break;
                                
                                default: /* [C][0][2]{!(0~2)} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }    
                            
                        }
                        break;

                        case 3: /* [C][0]{3}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 0: /* [C][0][3]{0} */
                                {                                    
                                    type     =  E_D97_RULER_METER_HAVA_CONSTANT;    /* ��Լ���� */                     
                                    format     =  E_D97_FORMAT_NNNNNN;   
                                    unit     =  E_D97_UNIT_FACTOR;                 
                                    rdwr     =  E_D97_RDWR_READ_WRITE;           
                                    length     =  3;                             
                                    func     =  trans_d97_format_NNNNNN;      
                                    pName     =  "�����(�й�)";                        
                                }
                                break;
                                
                                case 1: /* [C][0][3]{1} */
                                {                                    
                                    type     =  E_D97_RULER_METER_NONE_CONSTANT;        
                                    format     =  E_D97_FORMAT_NNNNNN;                  
                                    unit     =  E_D97_UNIT_FACTOR;                 
                                    rdwr     =  E_D97_RDWR_READ_WRITE;               
                                    length     =  3;                                 
                                    func     =  trans_d97_format_NNNNNN;      
                                    pName     =  "�����(�޹�)";                    
                                }
                                break;

                                case 2: /* [C][0][3]{2} */
                                {
                                    type     =  E_D97_RULER_METER_ID;                 /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN_6;                   
                                    unit     =  E_D97_UNIT_ID;                        
                                    rdwr     =  E_D97_RDWR_READ_WRITE;                   
                                    length     =  6;                                     
                                    func     =  trans_d97_format_NN_6;           
                                    pName     =  "���";                             
                                }
                                break;

                                case 3: /* [C][0][3]{3} */
                                {                                    
                                    type     =  E_D97_RULER_USER_ID;    /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN_6;   
                                    unit     =  E_D97_UNIT_ID;                 
                                    rdwr     =  E_D97_RDWR_READ_WRITE;           
                                    length     =  6;                             
                                    func     =  trans_d97_format_NN_6;      
                                    pName     =  "�û���";                        
                                }
                                break;

                                case 4: /* [C][0][3]{4} */
                                {                                    
                                    type     =  E_D97_RULER_DEV_CODE;          /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN_6;              
                                    unit     =  E_D97_UNIT_ID;                 
                                    rdwr     =  E_D97_RDWR_READ_WRITE;           
                                    length     =  6;                             
                                    func     =  trans_d97_format_NN_6;      
                                    pName     =  "�豸��";                        
                                }
                                break;
                                
                                default: /* [C][0][3]{!(0~4)} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }    
                            
                        }
                        break;

                        default: /* [C][0]{!(1~3)}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;

                    }
                }
                break;

                case 1: /* [C]{1}[*][*] */
                {
                    switch(ucDi01)
                    {
                        case 1: /* [C][1]{1}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 1: /* [C][1][1]{1} */
                                {                                    
                                    type     =  E_D97_RULER_DEMAND_PERIOD;     /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX;                  
                                    unit     =  E_D97_UNIT_mm;                
                                    rdwr     = E_D97_RDWR_READ_WRITE;           
                                    length     = 1;                             
                                    func     =  trans_d97_format_XX;        
                                    pName     =  "�����������";               
                                }
                                break;
                                
                                case 2: /* [C][1][1]{2} */
                                {                                    
                                    type     =  E_D97_RULER_SLIP_TIME;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX;                  
                                    unit     =  E_D97_UNIT_mm;                
                                    rdwr     = E_D97_RDWR_READ_WRITE;           
                                    length     = 1;                             
                                    func     =  trans_d97_format_XX;        
                                    pName     =  "����ʱ��";                   
                                }
                                break;

                                case 3: /* [C][1][1]{3} */
                                {                                    
                                    type     =  E_D97_RULER_DISPLAY_INTURN_TIME;         /* ��Լ���� */                                         
                                    format     =  E_D97_FORMAT_XX;                   
                                    unit     =  E_D97_UNIT_ss;                  
                                    rdwr     = E_D97_RDWR_READ_WRITE;            
                                    length     = 1;                              
                                    func     =  trans_d97_format_XX;         
                                    pName     =  "ѭ��ʱ��";                    
                                }
                                break;

                                case 4: /* [C][1][1]{4} */
                                {                                    
                                    type     =  E_D97_RULER_DISPLAY_HOLD_TIME;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_XX;                  
                                    unit     =  E_D97_UNIT_ss;                 
                                    rdwr     = E_D97_RDWR_READ_WRITE;           
                                    length     = 1;                             
                                    func     =  trans_d97_format_XX;        
                                    pName     =  "ͣ��ʱ��";                   
                                }
                                break;

                                case 5: /* [C][1][1]{5} */
                                {                                    
                                    type     =  E_D97_RULER_DISPLAY_POWER_DECIMAL_DIGITS;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN;                  
                                    unit     =  E_D97_UNIT_NUMBER;                 
                                    rdwr     =  E_D97_RDWR_READ_WRITE;           
                                    length     =  1;                             
                                    func     =  trans_d97_format_NN;        
                                    pName     =  "��ʾ����С��λ��";                   
                                }
                                break;

                                case 6: /* [C][1][1]{6} */
                                {                                    
                                    type     =  E_D97_RULER_DISPLAY_DEMAND_DECIMAL_DIGITS;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN;                                       
                                    unit     =  E_D97_UNIT_NUMBER;                                  
                                    rdwr     =  E_D97_RDWR_READ_WRITE;                                
                                    length     =  1;                                                
                                    func     =  trans_d97_format_NN;                                 
                                    pName     =  "��ʾ����(�������)С��λ��";                        
                                }
                                break;

                                case 7: /* [C][1][1]{7} */
                                {                                    
                                    type     =  E_D97_RULER_AUTO_READ_METER_DATE;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_DDhh;                          
                                    unit     =  E_D97_UNIT_DDhh;                           
                                    rdwr     =  E_D97_RDWR_READ_WRITE;                       
                                    length     =  2;                                         
                                    func     =  trans_d97_format_DDhh;                    
                                    pName     =  "�Զ���������";                               
                                }
                                break;

                                case 8: /* [C][1][1]{8} */
                                {                                    
                                    type     =  E_D97_RULER_LOAD_DELEGATE_DATE;            /* ��Լ���� */                                         
                                    format     =  E_D97_FORMAT_NN;                              
                                    unit     =  E_D97_UNIT_DD;                             
                                    rdwr     =  E_D97_RDWR_READ_WRITE;                       
                                    length     =  1;                                         
                                    func     =  trans_d97_format_NN;                        
                                    pName     =  "���ɴ�����";                               
                                }
                                break;

                                case 9: /* [C][1][1]{9} */
                                {
                                    type     =  E_D97_RULER_HAVE_POWER_START_DATA;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNNNNN_N;                      
                                    unit     =  E_D97_UNIT_KWH;                             
                                    rdwr     =  E_D97_RDWR_READ_WRITE;                        
                                    length     =  4;                                          
                                    func     =  trans_d97_format_NNNNNN_N;                 
                                    pName     =  "�й�������ʼ����";                         
                                }
                                break;

                                case 0xA: /* [C][1][1]{A} */
                                {
                                    type     =  E_D97_RULER_NONE_POWER_START_DATA;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNNNNN_N;                     
                                    unit     =  E_D97_UNIT_KWH;                             
                                    rdwr     =  E_D97_RDWR_READ_WRITE;                       
                                    length     =  4;                                         
                                    func     =  trans_d97_format_NNNNNN_N;                
                                    pName     =  "�޹�������ʼ����";                           
                                }
                                break;

                                default: /* [C][1][1]{!(1~A)} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }
                        }
                        break;

                        default: /* [C][0]{!(1)}[*] */
                            return E_D97_ERROR_ID_UNKNOWN;
                    }

                }
                break;

                case 2: /* [C]{2}[*][*] */
                {
                    switch(ucDi01)
                    {
                        case 1: /* [C][2]{1}[*] */
                        {
                            switch(ucDi00)
                            {
                                case 1:/* [C][2][1]{1} */
                                {
                                    type     =  E_D97_RULER_OUTPUT_PULSE_WIDTH;/* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNN;             
                                    unit     =  E_D97_UNIT_ms;                 
                                    rdwr     =  E_D97_RDWR_READ_WRITE;          
                                    length     =  2;                             
                                    func     =  trans_d97_format_NNNN;         
                                    pName     =  "���������";                
                                }
                                break;

                                case 2: /* [C][2][1]{2} */
                                {
                                    type     =  E_D97_RULER_PASSWORD;          /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NNNNNNNN;         
                                    unit     =  E_D97_UNIT_NUMBER;             
                                    rdwr     =  E_D97_RDWR_READ_WRITE;          
                                    length     =  4;                             
                                    func     =  trans_d97_format_NNNNNNNN;     
                                    pName     =  "����Ȩ�޼�����";              
                                }
                                break;
                                    
                                default: /* [C][2][1]{!(1~2)} */
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }
                        }
                        break;
                        
                        default: /* [C][2]{!(1)}[*]*/
                            return E_D97_ERROR_ID_UNKNOWN;
                        
                    }
                }
                break;

                case 3: /* [C]{3}[*][*] */
                {
                    switch(ucDi01)
                    {
                           case 1:/* [C][3]{1}[*]*/
                        {
                            switch(ucDi00)
                            {
                                case 0:/* [C][3][1]{0}*/
                                {                                    
                                    type     =  E_D97_RULER_YEAR_ZONE_P;    /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN;              
                                    unit     =  E_D97_UNIT_NUMBER;          
                                    rdwr     =  E_D97_RDWR_READ_WRITE;       
                                    length     =  1;                          
                                    func     =  trans_d97_format_NN;        
                                    pName     =  "��ʱ����P";                
                                }
                                break;
                                
                                case 1:/* [C][3][1]{1}*/
                                {                                    
                                    type     =  E_D97_RULER_DAY_ZONE_Q;        /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN;                  
                                    unit     =  E_D97_UNIT_NUMBER;              
                                    rdwr     = E_D97_RDWR_READ_WRITE;        
                                    length     = 1;                            
                                    func     =  trans_d97_format_NN;          
                                    pName     =  "��ʱ�α���q";               
                                }
                                break;
                                
                                case 2:/* [C][3][1]{2}*/
                                {                                    
                                    type     =  E_D97_RULER_DAY_ZONE_M;            /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN;                      
                                    unit     =  E_D97_UNIT_NUMBER;                  
                                    rdwr     =  E_D97_RDWR_READ_WRITE;               
                                    length     =  1;                                 
                                    func     =  trans_d97_format_NN;              
                                    pName     =  "��ʱ��(ÿ���л���)m <= 10";     
                                }
                                break;
                                
                                case 3:/* [C][3][1]{3}*/
                                {                                    
                                    type     =  E_D97_RULER_RATE_K;                /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN;                      
                                    unit     =  E_D97_UNIT_NUMBER;                  
                                    rdwr     =  E_D97_RDWR_READ_WRITE;               
                                    length     =  1;                                 
                                    func     =  trans_d97_format_NN;              
                                    pName     =  "������k <= 14";                   
                                }
                                break;
                                
                                case 4:/* [C][3][1]{4}*/
                                {                                    
                                    type     =  E_D97_RULER_HOLIDAY_NUMBER_N;    /* ��Լ���� */
                                    format     =  E_D97_FORMAT_NN;                      
                                    unit     =  E_D97_UNIT_NUMBER;                  
                                    rdwr     =  E_D97_RDWR_READ_WRITE;            
                                    length     =  1;                               
                                    func     =  trans_d97_format_NN;              
                                    pName     =  "����������n";                   
                                }
                                break;
                                
                                default: /* [C][2]{!(0~4)}[*]*/
                                    return E_D97_ERROR_ID_UNKNOWN;
                            }
                        }
                        break;

                        case 2:/* [C][3]{2}[*]*/
                        {
                            if(ucDi00 > g_year_p || ucDi00 > YEAR_P_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }

                            para.year_p = ucDi00;
                            type     =  E_D97_RULER_START_DATE_DAY_ZONE_P;              
                            format     =  E_D97_FORMAT_MMDDNN;                              
                            unit     =  E_D97_UNIT_NUMBER;                              
                            rdwr     =  E_D97_RDWR_READ_WRITE;                           
                            length     =  3;                                              
                            func     =  trans_d97_format_MMDDNN;                            
                            pName     =  "xʱ����ʼ���ڼ���ʱ�α��(x = 1-p)";           
                            
                        }
                        break;

                        case 3:/* [C][3]{3}[*]*/
                        {
                            if(ucDi00 > g_interval_m || ucDi00 > INTERVAL_M_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }
                            
                            para.interval_m = ucDi00;
                            type     =  E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M;       /* ��Լ���� */
                            format     =  E_D97_FORMAT_hhmmNN;                             
                            unit     =  E_D97_UNIT_NUMBER;                             
                            rdwr     =  E_D97_RDWR_READ_WRITE;                          
                            length     =  3;                                             
                            func     =  trans_d97_format_hhmmNN;                       
                            pName     =  "��һ��ʱ�α��mʱ����ʼʱ�估���ʺ�";         
                            
                        }
                        break;

                        case 4:/* [C][3]{4}[*]*/
                        {
                            if(ucDi00 > g_interval_m || ucDi00 > INTERVAL_M_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }
                            
                            para.interval_m = ucDi00;
                            type     =  E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M;       /* ��Լ���� */
                            format     =  E_D97_FORMAT_hhmmNN;                             
                            unit     =  E_D97_UNIT_NUMBER;                             
                            rdwr     =  E_D97_RDWR_READ_WRITE;                          
                            length     =  3;                                             
                            func     =  trans_d97_format_hhmmNN;                       
                            pName     =  "��2��ʱ�α��mʱ����ʼʱ�估���ʺ�";          
                        }
                        break;
                        
                        case 5:/* [C][3]{5}[*]*/
                        {
                            if(ucDi00 > g_interval_m || ucDi00 > INTERVAL_M_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }
                            
                            para.interval_m = ucDi00;
                            type     =  E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M;       /* ��Լ���� */
                            format     =  E_D97_FORMAT_hhmmNN;                             
                            unit     =  E_D97_UNIT_NUMBER;                             
                            rdwr     =  E_D97_RDWR_READ_WRITE;                          
                            length     =  3;                                             
                            func     =  trans_d97_format_hhmmNN;                       
                            pName     =  "��3��ʱ�α��mʱ����ʼʱ�估���ʺ�";          
                        }
                        break;

                        case 6:/* [C][3]{1}[*]*/
                        {
                            if(ucDi00 > g_interval_m || ucDi00 > INTERVAL_M_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }

                            para.interval_m = ucDi00;
                            type     =  E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M;           /* ��Լ���� */
                            format     =  E_D97_FORMAT_hhmmNN;                               
                            unit     =  E_D97_UNIT_NUMBER;                                  
                            rdwr     =  E_D97_RDWR_READ_WRITE;                            
                            length     =  3;                                                  
                            func     =  trans_d97_format_hhmmNN;                         
                            pName     =  "��4��ʱ�α��mʱ����ʼʱ�估���ʺ�";              
                        }
                        break;

                        case 7:/* [C][3]{2}[*]*/
                        {
                            if(ucDi00 > g_interval_m || ucDi00 > INTERVAL_M_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }

                            para.interval_m = ucDi00;
                            type     =  E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M;           /* ��Լ���� */
                            format     =  E_D97_FORMAT_hhmmNN;                               
                            unit     =  E_D97_UNIT_NUMBER;                                  
                            rdwr     =  E_D97_RDWR_READ_WRITE;                            
                            length     =  3;                                                  
                            func     =  trans_d97_format_hhmmNN;                         
                            pName     =  "��5��ʱ�α��mʱ����ʼʱ�估���ʺ�";              
                        }
                        break;

                        case 8:/* [C][3]{3}[*]*/
                        {
                            if(ucDi00 > g_interval_m || ucDi00 > INTERVAL_M_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }

                            para.interval_m = ucDi00;
                            type     =  E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M;       /* ��Լ���� */
                            format     =  E_D97_FORMAT_hhmmNN;                             
                            unit     =  E_D97_UNIT_NUMBER;                             
                            rdwr     =  E_D97_RDWR_READ_WRITE;                          
                            length     =  3;                                             
                            func     =  trans_d97_format_hhmmNN;                       
                            pName     =  "��6��ʱ�α��mʱ����ʼʱ�估���ʺ�";          
                        }
                        break;

                        case 9:/* [C][3]{4}[*]*/
                        {
                            if(ucDi00 > g_interval_m || ucDi00 > INTERVAL_M_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }

                            para.interval_m = ucDi00;
                            type     =  E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M;      /* ��Լ���� */
                            format     =  E_D97_FORMAT_hhmmNN;                            
                            unit     =  E_D97_UNIT_NUMBER;                            
                            rdwr     =  E_D97_RDWR_READ_WRITE;                         
                            length     =  3;                                            
                            func     =  trans_d97_format_hhmmNN;                      
                            pName     =  "��7��ʱ�α��mʱ����ʼʱ�估���ʺ�";         
                        }
                        break;
                        
                        case 0xA:/* [C][3]{5}[*]*/
                        {
                            if(ucDi00 > g_interval_m || ucDi00 > INTERVAL_M_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }

                            para.interval_m = ucDi00;                            
                            type     =  E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M;       /* ��Լ���� */
                            format     =  E_D97_FORMAT_hhmmNN;                             
                            unit     =  E_D97_UNIT_NUMBER;                             
                            rdwr     =  E_D97_RDWR_READ_WRITE;                          
                            length     =  3;                                             
                            func     =  trans_d97_format_hhmmNN;                       
                            pName     =  "��8��ʱ�α��mʱ����ʼʱ�估���ʺ�";          
                        }
                        break;

                        default :/* [C][3]{!(1~A)}[*]*/
                            return E_D97_ERROR_ID_UNKNOWN;
                        
                        
                    }
                }
                break;
                
                case 4: /* [C]{4}[*][*] */
                {
                    switch(ucDi01)
                    {
                        case 1: /* [C][4]{1}[*]*/
                        {
                            
                            if(ucDi00 == 0 || ucDi00 > g_holiday_n || ucDi00 > HOLIDAY_N_MAX)
                            {
                                return E_D97_ERROR_ID_UNKNOWN;
                            }
                            
                            if(ucDi00 == 0xE)
                            {
                                para.holiday_n = ucDi00;
                                type     =  E_D97_RULER_WEEKS_DAY_ZONE;       
                                format     =  E_D97_FORMAT_NN;                    
                                unit     =  E_D97_UNIT_NUMBER;                
                                rdwr     =  E_D97_RDWR_READ_WRITE;             
                                length     =  1;                                
                                func     =  trans_d97_format_NN;              
                                pName     =  "�����ղ��õ�ʱʱ�α��";         
                            }
                            else
                            {
                                para.holiday_n = ucDi00;
                                type     =  E_D97_RULER_DATE_DAY_ZONE_HOLIDAY_N;       
                                format     =  E_D97_FORMAT_MMDDNN;                         
                                unit     =  E_D97_UNIT_NUMBER;                         
                                rdwr     =  E_D97_RDWR_READ_WRITE;                      
                                length     =  3;                                         
                                func     =  trans_d97_format_MMDDNN;                          
                                pName     =  "��(n)�����������ڼ���ʱ�α��";           
                            }
                        }
                        break;

                        default:/* [C][4]{!(1)}[*]*/
                          return E_D97_ERROR_ID_UNKNOWN;    

                    }
                }
                break;
                
                case 5: /* [C]{5}[*][*] */
                {
                    if(ucDi01 == 1)  /* [C][5]{1}[*] */
                    {
                        switch(ucDi00) 
                        {
                            case 0: /* [C][5][1]{0} */
                            {
                                type     =  E_D97_RULER_LOAD_RECORD_START_TIME;       
                                format     =  E_D97_FORMAT_MMDDhhmm;                               
                                unit     =  E_D97_UNIT_MMDDhhmm;                              
                                rdwr     =  E_D97_RDWR_READ_WRITE;                            
                                length     =  4;                                              
                                func     =  trans_d97_format_MMDDhhmm;                     
                                pName     =  "���ɼ�¼��ʼʱ��";          
                            }
                            break;

                            case 1: /* [C][5][1]{1} */
                            {
                                type     =  E_D97_RULER_LOAD_RECORD_PERIOD_TIME;       
                                format     =  E_D97_FORMAT_NNNN;                               
                                unit     =  E_D97_UNIT_mm;                              
                                rdwr     = E_D97_RDWR_READ_WRITE;                            
                                length     = 2;                                              
                                func     =  trans_d97_format_NNNN;                                
                                pName     =  "���ɼ�¼���ʱ��";          
                            }
                            break;

                            default:  /* [C][5][1]{!(0~1)} */
                                 return E_D97_ERROR_ID_UNKNOWN;    
                            
                        }
                    }
                    else /* [C][5]{!(1)}[*] */
                    {
                        return E_D97_ERROR_ID_UNKNOWN;
                    }
                }
                break;
                       
                default: /* [C]{!(0~5)}[*][*] */
                    return E_D97_ERROR_ID_UNKNOWN;
            }
        }
       break;


       case 0xD: /* {D}[*][*][*] */
           {
            switch(ucDi10)
            {
                case 1: /* [D]{1}[*][*] */
                {
                    if(ucDi10 == 1 && ucDi00 == 0) /*[D][1][1][0]*/
                    {
                        type     =  E_D97_RULER_CURRENT_POWER_LOAD_BLOCK;      
                        format  =  E_D97_FORMAT_MMDDNN;                         
                        unit     =  E_D97_UNIT_mm;                             
                        rdwr    =  E_D97_RDWR_READ_WRITE;                      
                        length     =  2;                                         
                        func    =  trans_d97_format_MMDDNN;                       
                        pName   =  "��ǰ�����й��ܵ��ܸ��ɼ�¼���ݿ�";        
                    }
                    else /*[D][1]{!(1)}{!(0)}*/
                    {
                        return E_D97_ERROR_ID_UNKNOWN;
                    }
                }
                break;

                case 2: /* [D]{2}[*][*] */
                {
                    if(ucDi01 == 1 && ucDi00 == 0) /* [D][2][1][0] */
                    {
                        type     =  E_D97_RULER_CURRENT_DEMAND_LOAD_BLOCK;     
                        format     =  E_D97_FORMAT_MMDDNN;                        
                        unit     =  E_D97_UNIT_mm;                            
                        rdwr     =  E_D97_RDWR_READ_WRITE;                       
                        length     =  2;                                          
                        func     =  trans_d97_format_MMDDNN;                     
                        pName     =  "��ǰ��������������ɼ�¼���ݿ�";          
                    }
                    else /* [D][2]{!(1)}{!(0)} */
                    {
                        return E_D97_ERROR_ID_UNKNOWN;
                    }
                }
                break;

                default: /* []{!(1~2)}[*][*] */
                    return E_D97_ERROR_ID_UNKNOWN;
                
            }
        }
        break;

        /*�û���չ*/
        case 0xE:
        {
        }
        break;

         /* û�ж���Ĺ�Լ ID */
        default:
            return E_D97_ERROR_ID_UNKNOWN;

    }

 


    /* ������� */
    outRulerInfo->type     = type;

    /*para*/
    outRulerInfo->para.checkPoint = para.checkPoint;
    outRulerInfo->para.day_q = para.day_q;
    outRulerInfo->para.holiday_n = para.holiday_n;
    outRulerInfo->para.interval_m = para.interval_m;
    outRulerInfo->para.rate_k = para.rate_k;
    outRulerInfo->para.year_p = para.year_p;
    outRulerInfo->format     = format;
    outRulerInfo->unit     = unit;    
    outRulerInfo->rdwr     = rdwr;
    outRulerInfo->length     = length;
    outRulerInfo->func     = func;
    sprintf(outRulerInfo->name,"%s", pName);
    return E_D97_OK;
}



/*************************************************
Function:       get_d97_ruler_id
Description:    ͨ����Լ���ͺ�һЩ���������þ���
                �Ĺ�ԼID

Author:         liming 

Calls:          
Called By:      
Input:          E_D97_RULER_TYPE rulerType ��Լ����
                S_D97_RULER_TYPE_PARA para �������
Output:         poutRulerID �����������ԼID
Return:         ��ȷ���� 0 ����Ϊ��������

Others:         para �е���������ÿ�ζ���Ҫ
  
*************************************************/
eD97Err get_d97_ruler_id(E_D97_RULER_TYPE rulerType, 
                      S_D97_RULER_TYPE_PARA para,
                      UINT16 *poutRulerID)
{

    UINT16 usRulerID = 0;    
    UINT8  ucDi0 = 0;
    UINT8  ucDi1 = 0;
    UINT8  ucDi10 = 0;
    UINT8  ucDi11 = 0;
    UINT8  ucDi00 = 0;
    UINT8  ucDi01 = 0;

    /* ������ ���������ʱ�� �����Ӧ����� A1-A3 */
    if(rulerType <= E_D97_RULER_STANDARD_CONSTANT_MAX)
    {
        /* Di11 */
        if(rulerType <= E_D97_RULER_STANDARD_POWER_MAX)
        {
            ucDi11 = 9;
        }
        else if(rulerType >= E_D97_RULER_STANDARD_DEMAND_MIN &&
                rulerType <= E_D97_RULER_STANDARD_DEMAND_MAX)
        {
            ucDi11 = 0xA;

        }
        else
        {
            ucDi11 = 0xB;
        }
        
        /* Di10  Di01 */
        switch(rulerType)
        {
            /* 1 */
            case E_D97_RULER_FORTH_HAVE_POWER_TOTAL:
            case E_D97_RULER_FORTH_HAVE_POWER_RATE:
            case E_D97_RULER_FORTH_HAVE_POWER_BLOCK:
            case E_D97_RULER_FORTH_HAVE_DEMAND_TOTAL:
            case E_D97_RULER_FORTH_HAVE_DEMAND_RATE:
            case E_D97_RULER_FORTH_HAVE_DEMAND_BLOCK:
            case E_D97_RULER_FORTH_HAVE_DEMAND_TIME_TOTAL:
            case E_D97_RULER_FORTH_HAVE_DEMAND_TIME_RATE:
            case E_D97_RULER_FORTH_HAVE_DEMAND_TIME_BLOCK:

            if(E_D97_CHECK_POINT_CURRENT == para.checkPoint)
            {
                /* ��ǰ */
                ucDi10 = 0;
                ucDi01 = 1;
            }
            else if(E_D97_CHECK_POINT_LAST_MONTH == para.checkPoint)    
            {
                /*����*/
                ucDi10 = 4;
                ucDi01 = 1;
            }
            else if(E_D97_CHECK_POINT_LAST_LAST_MONTH == para.checkPoint )
            {
                /*������*/
                ucDi10 = 8;
                ucDi01 = 1;
            }
            
            break;

            /* 2 */
            case E_D97_RULER_BACK_HAVE_POWER_TOTAL:
            case E_D97_RULER_BACK_HAVE_POWER_RATE:
            case E_D97_RULER_BACK_HAVE_POWER_BLOCK:
            case E_D97_RULER_BACK_HAVE_DEMAND_TOTAL:
            case E_D97_RULER_BACK_HAVE_DEMAND_RATE:
            case E_D97_RULER_BACK_HAVE_DEMAND_BLOCK:
            case E_D97_RULER_BACK_HAVE_DEMAND_TIME_TOTAL:
            case E_D97_RULER_BACK_HAVE_DEMAND_TIME_RATE:
            case E_D97_RULER_BACK_HAVE_DEMAND_TIME_BLOCK:

            if(E_D97_CHECK_POINT_CURRENT == para.checkPoint)
            {
                /* ��ǰ */
                ucDi10 = 0;
                ucDi01 = 2;
            }
            else if(E_D97_CHECK_POINT_LAST_MONTH == para.checkPoint)    
            {
                /*����*/
                ucDi10 = 4;
                ucDi01 = 2;
            }
            else if(E_D97_CHECK_POINT_LAST_LAST_MONTH == para.checkPoint )
            {
                /*������*/
                ucDi10 = 8;
                ucDi01 = 2;
            }
            
            break;

            /* 3 */
            case E_D97_RULER_FORTH_NONE_POWER_TOTAL:
            case E_D97_RULER_FORTH_NONE_POWER_RATE:
            case E_D97_RULER_FORTH_NONE_POWER_BLOCK:
            case E_D97_RULER_FORTH_NONE_DEMAND_TOTAL:
            case E_D97_RULER_FORTH_NONE_DEMAND_RATE:
            case E_D97_RULER_FORTH_NONE_DEMAND_BLOCK:
            case E_D97_RULER_FORTH_NONE_DEMAND_TIME_TOTAL:
            case E_D97_RULER_FORTH_NONE_DEMAND_TIME_RATE:
            case E_D97_RULER_FORTH_NONE_DEMAND_TIME_BLOCK:

            if(E_D97_CHECK_POINT_CURRENT == para.checkPoint)
            {
                /* ��ǰ */
                ucDi10 = 1;
                ucDi01 = 1;
            }
            else if(E_D97_CHECK_POINT_LAST_MONTH == para.checkPoint)    
            {
                /*����*/
                ucDi10 = 5;
                ucDi01 = 1;
            }
            else if(E_D97_CHECK_POINT_LAST_LAST_MONTH == para.checkPoint )
            {
                /*������*/
            }
            
            break;
                
            /* 4 */
            case E_D97_RULER_BACK_NONE_POWER_TOTAL:
            case E_D97_RULER_BACK_NONE_POWER_RATE:
            case E_D97_RULER_BACK_NONE_POWER_BLOCK:
            case E_D97_RULER_BACK_NONE_DEMAND_TOTAL:
            case E_D97_RULER_BACK_NONE_DEMAND_RATE:
            case E_D97_RULER_BACK_NONE_DEMAND_BLOCK:
            case E_D97_RULER_BACK_NONE_DEMAND_TIME_TOTAL:
            case E_D97_RULER_BACK_NONE_DEMAND_TIME_RATE:
            case E_D97_RULER_BACK_NONE_DEMAND_TIME_BLOCK:

            if(E_D97_CHECK_POINT_CURRENT == para.checkPoint)
            {
                /* ��ǰ */
                ucDi10 = 1;
                ucDi01 = 2;
            }
            else if(E_D97_CHECK_POINT_LAST_MONTH == para.checkPoint)    
            {
                /*����*/
                ucDi10 = 5;
                ucDi01 = 2;
            }
            else if(E_D97_CHECK_POINT_LAST_LAST_MONTH == para.checkPoint )
            {
                /*������*/
                ucDi10 = 9;
                ucDi01 = 2;
            }
            
            break;

            /* 5 */
            case E_D97_RULER_QUAD_1_NONE_POWER_TOTAL: 
            case E_D97_RULER_QUAD_1_NONE_POWER_RATE:  
            case E_D97_RULER_QUAD_1_NONE_POWER_BLOCK:  
            case E_D97_RULER_QUAD_1_NONE_DEMAND_TOTAL:  
            case E_D97_RULER_QUAD_1_NONE_DEMAND_RATE:  
            case E_D97_RULER_QUAD_1_NONE_DEMAND_BLOCK:
            case E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_TOTAL: 
            case E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_RATE:  
            case E_D97_RULER_QUAD_1_NONE_DEMAND_TIME_BLOCK: 
                
            if(E_D97_CHECK_POINT_CURRENT == para.checkPoint)
            {
                /* ��ǰ */
                ucDi10 = 1;
                ucDi01 = 3;
            }
            else if(E_D97_CHECK_POINT_LAST_MONTH == para.checkPoint)    
            {
                /*����*/
                ucDi10 = 5;
                ucDi01 = 3;
            }
            else if(E_D97_CHECK_POINT_LAST_LAST_MONTH == para.checkPoint )
            {
                /*������*/
                ucDi10 = 9;
                ucDi01 = 3;
            }
            
            break;

            /* 6 */
            case E_D97_RULER_QUAD_4_NONE_POWER_TOTAL: 
            case E_D97_RULER_QUAD_4_NONE_POWER_RATE:  
            case E_D97_RULER_QUAD_4_NONE_POWER_BLOCK:  
            case E_D97_RULER_QUAD_4_NONE_DEMAND_TOTAL:  
            case E_D97_RULER_QUAD_4_NONE_DEMAND_RATE:  
            case E_D97_RULER_QUAD_4_NONE_DEMAND_BLOCK:
            case E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_TOTAL: 
            case E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_RATE:  
            case E_D97_RULER_QUAD_4_NONE_DEMAND_TIME_BLOCK: 

            if(E_D97_CHECK_POINT_CURRENT == para.checkPoint)
            {
                /* ��ǰ */
                ucDi10 = 1;
                ucDi01 = 4;
            }
            else if(E_D97_CHECK_POINT_LAST_MONTH == para.checkPoint)    
            {
                /*����*/
                ucDi10 = 5;
                ucDi01 = 4;
            }
            else if(E_D97_CHECK_POINT_LAST_LAST_MONTH == para.checkPoint )
            {
                /*������*/
                ucDi10 = 9;
                ucDi01 = 4;
            }
            
            break;

            /* 7 */
            case E_D97_RULER_QUAD_2_NONE_POWER_TOTAL: 
            case E_D97_RULER_QUAD_2_NONE_POWER_RATE:  
            case E_D97_RULER_QUAD_2_NONE_POWER_BLOCK:  
            case E_D97_RULER_QUAD_2_NONE_DEMAND_TOTAL:  
            case E_D97_RULER_QUAD_2_NONE_DEMAND_RATE:  
            case E_D97_RULER_QUAD_2_NONE_DEMAND_BLOCK:
            case E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_TOTAL: 
            case E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_RATE:  
            case E_D97_RULER_QUAD_2_NONE_DEMAND_TIME_BLOCK: 

            if(E_D97_CHECK_POINT_CURRENT == para.checkPoint)
            {
                /* ��ǰ */
                ucDi10 = 1;
                ucDi01 = 5;
            }
            else if(E_D97_CHECK_POINT_LAST_MONTH == para.checkPoint)    
            {
                /*����*/
                ucDi10 = 5;
                ucDi01 = 5;
            }
            else if(E_D97_CHECK_POINT_LAST_LAST_MONTH == para.checkPoint )
            {
                /*������*/
                ucDi10 = 9;
                ucDi01 = 5;
            }
            
            break;
            
            /* 8 */
            case E_D97_RULER_QUAD_3_NONE_POWER_TOTAL: 
            case E_D97_RULER_QUAD_3_NONE_POWER_RATE:  
            case E_D97_RULER_QUAD_3_NONE_POWER_BLOCK:  
            case E_D97_RULER_QUAD_3_NONE_DEMAND_TOTAL:  
            case E_D97_RULER_QUAD_3_NONE_DEMAND_RATE:  
            case E_D97_RULER_QUAD_3_NONE_DEMAND_BLOCK:
            case E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_TOTAL: 
            case E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_RATE:  
            case E_D97_RULER_QUAD_3_NONE_DEMAND_TIME_BLOCK: 

            if(E_D97_CHECK_POINT_CURRENT == para.checkPoint)
            {
                /* ��ǰ */
                ucDi10 = 1;
                ucDi01 = 6;
            }
            else if(E_D97_CHECK_POINT_LAST_MONTH == para.checkPoint)    
            {
                /*����*/
                ucDi10 = 5;
                ucDi01 = 6;
            }
            else if(E_D97_CHECK_POINT_LAST_LAST_MONTH == para.checkPoint )
            {
                /*������*/
                ucDi10 = 9;
                ucDi01 = 6;
            }
            
            break;
            
            default:

                return E_D97_ERROR_PARA_NONE;
                
        }

        
        /* Di00 */
        if(rulerType == E_D97_RULER_FORTH_HAVE_POWER_TOTAL  ||
           rulerType == E_D97_RULER_BACK_HAVE_POWER_TOTAL   ||
           rulerType == E_D97_RULER_FORTH_NONE_POWER_TOTAL  ||
           rulerType == E_D97_RULER_BACK_NONE_POWER_TOTAL   ||
           rulerType == E_D97_RULER_QUAD_1_NONE_POWER_TOTAL ||
           rulerType == E_D97_RULER_QUAD_4_NONE_POWER_TOTAL ||
           rulerType == E_D97_RULER_QUAD_2_NONE_POWER_TOTAL ||
           rulerType == E_D97_RULER_QUAD_3_NONE_POWER_TOTAL  )
        {
            ucDi00 = 0;
        }
        else if(rulerType == E_D97_RULER_FORTH_HAVE_POWER_RATE  ||
                 rulerType == E_D97_RULER_BACK_HAVE_POWER_RATE   ||
                 rulerType == E_D97_RULER_FORTH_NONE_POWER_RATE  ||
                 rulerType == E_D97_RULER_BACK_NONE_POWER_RATE   ||
                 rulerType == E_D97_RULER_QUAD_1_NONE_POWER_RATE ||
                 rulerType == E_D97_RULER_QUAD_4_NONE_POWER_RATE ||
                 rulerType == E_D97_RULER_QUAD_2_NONE_POWER_RATE ||
                 rulerType == E_D97_RULER_QUAD_3_NONE_POWER_RATE  )
        {
            if(0 == para.rate_k || 14 < para.rate_k)
            {
                return E_D97_ERROR_PARA_OUT_RANGE;
            }
             
            ucDi00 = para.rate_k;
        }
        else
        {
            /* ���ݿ� */
            ucDi00 = 0xF;
        }            
    }
    /* ���������� ����� ��Ӧ�� ��A.4 ~ A.6*/
    else if(rulerType >= E_D97_RULER_STANDARD_VAR_MIN ||
            rulerType <= E_D97_RULER_STANDARD_VAR_MAX)
    {


        /* Di11 */
        if(rulerType >= E_D97_RULER_RECENT_PROGRAME_TIME &&
           rulerType <= E_D97_RULER_PHASE_C_POWER_RATE_FACTOR)
        {
           ucDi11 = 0xB;
        }
        else if(rulerType >= E_D97_RULER_DATE_AND_WEEK_NUMBER &&
                rulerType <= E_D97_RULER_LOAD_RECORD_PERIOD_TIME)
        {
            ucDi11 = 0xC;
        }
        else
        {
            ucDi11 = 0xD;
        }

        /* Di10 Di01 Di00 */
        switch(rulerType)
        {
            /* A.4 */
            case E_D97_RULER_RECENT_PROGRAME_TIME:
                 ucDi10 = 2;
                 ucDi01 = 1;
                 ucDi00 = 0;
                break;
                
            case E_D97_RULER_RECENT_CLEAR_DEMAND_TIME:
                 ucDi10 = 2;
                 ucDi01 = 1;
                 ucDi00 = 1;
                break;
                
            case E_D97_RULER_PROGRAME_TIMES:
                 ucDi10 = 2;
                 ucDi01 = 1;
                 ucDi00 = 2;
                break;
                
            case E_D97_RULER_CLEAR_DEMAND_TIMES:
                 ucDi10 = 2;
                 ucDi01 = 1;
                 ucDi00 = 3;
                break;
                    
            case E_D97_RULER_CELL_WORK_TIMES:
                 ucDi10 = 2;
                 ucDi01 = 1;
                 ucDi00 = 4;
                break;
                
            case E_D97_RULER_PHASE_LOSS_TOTAL_TIMES:
                 ucDi10 = 3;
                 ucDi01 = 1;
                 ucDi00 = 0;
                break;
                
            case E_D97_RULER_PHASE_A_LOSS_TIMES:
                 ucDi10 = 3;
                 ucDi01 = 1;
                 ucDi00 = 1;
                break;
                
            case E_D97_RULER_PHASE_B_LOSS_TIMES:
                 ucDi10 = 3;
                 ucDi01 = 1;
                 ucDi00 = 2;
                break;
                
            case E_D97_RULER_PHASE_C_LOSS_TIMES:
                 ucDi10 = 3;
                 ucDi01 = 1;
                 ucDi00 = 3;
                break;

            case E_D97_RULER_PHASE_LOSS_SPEND:
                 ucDi10 = 3;
                 ucDi01 = 2;
                 ucDi00 = 0;
                 break;

            case E_D97_RULER_PHASE_A_LOSS_SPEND:
                 ucDi10 = 3;
                 ucDi01 = 2;
                 ucDi00 = 1;
                 break;
                
            case E_D97_RULER_PHASE_B_LOSS_SPEND:
                 ucDi10 = 3;
                 ucDi01 = 2;
                 ucDi00 = 2;
                break;
                
            case E_D97_RULER_PHASE_C_LOSS_SPEND:
                 ucDi10 = 3;
                 ucDi01 = 2;
                 ucDi00 = 3;
                break;
                
            case E_D97_RULER_RECENT_PHASE_LOSS_START_TIME:
                 ucDi10 = 3;
                 ucDi01 = 3;
                 ucDi00 = 0;
                break;

            case E_D97_RULER_RECENT_PHASE_A_LOSS_START_TIME:
                 ucDi10 = 3;
                 ucDi01 = 3;
                 ucDi00 = 1;
                break;
                
            case E_D97_RULER_RECENT_PHASE_B_LOSS_START_TIME:
                 ucDi10 = 3;
                 ucDi01 = 3;
                 ucDi00 = 2;
                 break;
                
            case E_D97_RULER_RECENT_PHASE_C_LOSS_START_TIME:
                 ucDi10 = 3;
                 ucDi01 = 3;
                 ucDi00 = 3;
                 break;
                
            case E_D97_RULER_RECENT_PHASE_LOSS_END_TIME:
                 ucDi10 = 3;
                 ucDi01 = 4;
                 ucDi00 = 0;
                 break;
                 
            case E_D97_RULER_RECENT_PHASE_A_LOSS_END_TIME:
                 ucDi10 = 3;
                 ucDi01 = 4;
                 ucDi00 = 1;
                 break;
                 
            case E_D97_RULER_RECENT_PHASE_B_LOSS_END_TIME:
                 ucDi10 = 3;
                 ucDi01 = 4;
                 ucDi00 = 2;
                 break;
                    
            case E_D97_RULER_RECENT_PHASE_C_LOSS_END_TIME:
                 ucDi10 = 3;
                 ucDi01 = 4;
                 ucDi00 = 3;
                 break;
                
            case E_D97_RULER_PHASE_A_VOLT:
                 ucDi10 = 6;
                 ucDi01 = 1;
                 ucDi00 = 1;
                 break;
                 
            case E_D97_RULER_PHASE_B_VOLT:
                 ucDi10 = 6;
                 ucDi01 = 1;
                 ucDi00 = 2;
                 break;
                
            case E_D97_RULER_PHASE_C_VOLT:
                 ucDi10 = 6;
                 ucDi01 = 1;
                 ucDi00 = 3;
                 break;
                
            case E_D97_RULER_PHASE_A_ELEC:
                 ucDi10 = 6;
                 ucDi01 = 2;
                 ucDi00 = 1;
                 break;
                
            case E_D97_RULER_PHASE_B_ELEC:
                 ucDi10 = 6;
                 ucDi01 = 2;
                 ucDi00 = 2;
                 break;
                
            case E_D97_RULER_PHASE_C_ELEC:
                 ucDi10 = 6;
                 ucDi01 = 2;
                 ucDi00 = 3;
                 break;
                 
            case E_D97_RULER_INSTANT_HAVE_POWER_RATE:
                 ucDi10 = 6;
                 ucDi01 = 3;
                 ucDi00 = 0;
                 break;

            case E_D97_RULER_PHASE_A_HAVE_POWER_RATE:
                 ucDi10 = 6;
                 ucDi01 = 3;
                 ucDi00 = 1;
                 break;

            case E_D97_RULER_PHASE_B_HAVE_POWER_RATE:
                 ucDi10 = 6;
                 ucDi01 = 3;
                 ucDi00 = 2;
                 break;
                
            case E_D97_RULER_PHASE_C_HAVE_POWER_RATE:
                 ucDi10 = 6;
                 ucDi01 = 3;
                 ucDi00 = 3;
                 break;
                
            case E_D97_RULER_FORTH_HAVE_POWER_RATE_LIMIT:
                 ucDi10 = 6;
                 ucDi01 = 3;
                 ucDi00 = 4;
                 break;
                
            case E_D97_RULER_BACK_HAVE_POWER_RATE_LIMIT:
                 ucDi10 = 6;
                 ucDi01 = 3;
                 ucDi00 = 5;
                 break;

            case E_D97_RULER_INSTANT_NONE_POWER_RATE:
                 ucDi10 = 6;
                 ucDi01 = 4;
                 ucDi00 = 0;
                 break;

            case E_D97_RULER_PHASE_A_NONE_POWER_RATE:
                 ucDi10 = 6;
                 ucDi01 = 4;
                 ucDi00 = 1;
                 break;

            case E_D97_RULER_PHASE_B_NONE_POWER_RATE:
                 ucDi10 = 6;
                 ucDi01 = 4;
                 ucDi00 = 2;
                 break;
                
            case E_D97_RULER_PHASE_C_NONE_POWER_RATE:
                 ucDi10 = 6;
                 ucDi01 = 4;
                 ucDi00 = 3;
                 break;
                
            case E_D97_RULER_TOTAL_POWER_RATE_FACTOR:
                 ucDi10 = 6;
                 ucDi01 = 5;
                 ucDi00 = 0;
                 break;
                
            case E_D97_RULER_PHASE_A_POWER_RATE_FACTOR:
                 ucDi10 = 6;
                 ucDi01 = 5;
                 ucDi00 = 1;
                 break;
                 
            case E_D97_RULER_PHASE_B_POWER_RATE_FACTOR:
                 ucDi10 = 6;
                 ucDi01 = 5;
                 ucDi00 = 2;
                 break;
                
            case E_D97_RULER_PHASE_C_POWER_RATE_FACTOR:
                 ucDi10 = 6;
                 ucDi01 = 5;
                 ucDi00 = 3;
                 break;

             /* A.5 */
            case E_D97_RULER_DATE_AND_WEEK_NUMBER:
                 ucDi10 = 0;
                 ucDi01 = 1;
                 ucDi00 = 0;
                 break;
                    
            case E_D97_RULER_TIME:
                 ucDi10 = 0;
                 ucDi01 = 1;
                 ucDi00 = 1;
                 break;
                
            case E_D97_RULER_STWD_METER_RUN:
                 ucDi10 = 0;
                 ucDi01 = 2;
                 ucDi00 = 0;
                 break;
                 
            case E_D97_RULER_STWD_NETWIRE_RUN:
                 ucDi10 = 0;
                 ucDi01 = 2;
                 ucDi00 = 1;
                 break;
                
            case E_D97_RULER_STWD_WEEK_DAY:
                 ucDi10 = 0;
                 ucDi01 = 2;
                 ucDi00 = 2;
                 break;
                
            case E_D97_RULER_METER_HAVA_CONSTANT:
                 ucDi10 = 0;
                 ucDi01 = 3;
                 ucDi00 = 0;
                 break;
                
            case E_D97_RULER_METER_NONE_CONSTANT:
                 ucDi10 = 0;
                 ucDi01 = 3;
                 ucDi00 = 1;
                 break;
                
            case E_D97_RULER_METER_ID:
                 ucDi10 = 0;
                 ucDi01 = 3;
                 ucDi00 = 2;
                 break;
                 
            case E_D97_RULER_USER_ID:
                 ucDi10 = 0;
                 ucDi01 = 3;
                 ucDi00 = 3;
                 break;

            case E_D97_RULER_DEV_CODE:
                 ucDi10 = 0;
                 ucDi01 = 3;
                 ucDi00 = 4;
                 break;

            case E_D97_RULER_DEMAND_PERIOD:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 1;
                 break;
                
            case E_D97_RULER_SLIP_TIME:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 2;
                 break;
                
            case E_D97_RULER_DISPLAY_INTURN_TIME:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 3;
                 break;
                
            case E_D97_RULER_DISPLAY_HOLD_TIME:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 4;
                 break;

            case E_D97_RULER_DISPLAY_POWER_DECIMAL_DIGITS:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 5;
                 break;

            case E_D97_RULER_DISPLAY_DEMAND_DECIMAL_DIGITS:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 6;
                 break;

            case E_D97_RULER_AUTO_READ_METER_DATE:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 7;
                 break;
                
            case E_D97_RULER_LOAD_DELEGATE_DATE:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 8;
                 break;
                
            case E_D97_RULER_HAVE_POWER_START_DATA:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 9;
                 break;
                
            case E_D97_RULER_NONE_POWER_START_DATA:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 0xA;
                 break;
                 
            case E_D97_RULER_OUTPUT_PULSE_WIDTH:
                 ucDi10 = 2;
                 ucDi01 = 1;
                 ucDi00 = 1;
                 break;
                
            case E_D97_RULER_PASSWORD:
                 ucDi10 = 2;
                 ucDi01 = 1;
                 ucDi00 = 2;
                 break;

            case E_D97_RULER_YEAR_ZONE_P:
                 ucDi10 = 3;
                 ucDi01 = 1;
                 ucDi00 = 0;
                 break;
                
            case E_D97_RULER_DAY_ZONE_Q:
                 ucDi10 = 3;
                 ucDi01 = 1;
                 ucDi00 = 1;
                 break;
                 
            case E_D97_RULER_DAY_ZONE_M:
                 ucDi10 = 3;
                 ucDi01 = 1;
                 ucDi00 = 2;
                 break;

            case E_D97_RULER_RATE_K:
                 ucDi10 = 3;
                 ucDi01 = 1;
                 ucDi00 = 3;
                 break;

            case E_D97_RULER_HOLIDAY_NUMBER_N:
                 ucDi10 = 3;
                 ucDi01 = 1;
                 ucDi00 = 4;
                 break;
                
            case E_D97_RULER_START_DATE_DAY_ZONE_P:
                 ucDi10 = 3;
                 ucDi01 = 2;
                 ucDi00 = para.year_p;
                 
                 if(0 == ucDi00 || g_year_p < ucDi00)
                 {
                     return E_D97_ERROR_PARA_OUT_RANGE;
                 }
                 
                 break;
                
            case E_D97_RULER_START_TIME_RATE_TH_Q_ZONE_M:
                 {
                     int m,q;
                    q = para.day_q;
                    m = para.interval_m;

                    if(q < 1 || q > g_day_q || q > DAY_Q_MAX ||
                       m < 1 || m > g_interval_m ||m > INTERVAL_M_MAX)
                    {
                        return E_D97_ERROR_PARA_OUT_RANGE; 
                    }
                    
                    ucDi10 = 3;
                    ucDi01 = q+2;                 
                    ucDi00 = m ;
                  }
            
                 break;
                
            case E_D97_RULER_DATE_DAY_ZONE_HOLIDAY_N:
                 {
                     int n;
                    n = para.holiday_n;
                    
                     if(n < 1 || n > g_holiday_n || n > HOLIDAY_N_MAX)
                     {
                        return E_D97_ERROR_PARA_OUT_RANGE;  
                     }
                    
                    ucDi10 = 4;
                    ucDi01 = 1;
                    ucDi00 = n;
                  }
            
                 break;

            case E_D97_RULER_WEEKS_DAY_ZONE:
                 ucDi10 = 4;
                 ucDi01 = 1;
                 ucDi00 = 0xE;
                 break;

            case E_D97_RULER_LOAD_RECORD_START_TIME:
                 ucDi10 = 5;
                 ucDi01 = 1;
                 ucDi00 = 0;
                 break;

            case E_D97_RULER_LOAD_RECORD_PERIOD_TIME:
                 ucDi10 = 5;
                 ucDi01 = 1;
                 ucDi00 = 1;
                 break;
                
            case E_D97_RULER_CURRENT_POWER_LOAD_BLOCK:
                 ucDi10 = 1;
                 ucDi01 = 1;
                 ucDi00 = 0;
                 break;
                
            case E_D97_RULER_CURRENT_DEMAND_LOAD_BLOCK:
                 ucDi10 = 2;
                 ucDi01 = 1;
                 ucDi00 = 0;
                 break;
                
            default:
                
                return E_D97_ERROR_PARA_NONE;


                 
        }
        
    }
    /* �û���չ��Լ */
    else if(rulerType >= E_D97_RULER_USER_EXTEND_MIN ||
            rulerType >= E_D97_RULER_USER_EXTEND_MAX)
    {
        

        
    }
    else
    {
        return E_D97_ERROR_PARA_NONE;
    }


    /* ��Ϲ�ԼID */     
    ucDi0 = (ucDi01 << 4) + ucDi00;
    ucDi1 = (ucDi11 << 4) + ucDi10;
    usRulerID = (ucDi1 << 8) + ucDi0;
    
    /* ������� */
    *poutRulerID = usRulerID;
     
    return E_D97_OK;

}



/*************************************************
Function:       trans_d97ctrl_struct2char
Description:    ����������ṹ �����������ֽ�ֵ���
                
Author:         liming 

Calls:          
Called By:      
Input:          S_D97_CTRL_CODE *inStruct

Output:         UINT8 *outChar
Return:      

Others:        ��ת�� trans_d97ctrl_char2struct
  
*************************************************/
eD97Err trans_d97ctrl_struct2char(UINT8 *outChar, S_D97_CTRL_CODE *inStruct)
{
    UINT8 ucCtrlCode = 0;
    UINT8 ucD7,ucD6,ucD5,ucD40;

    if(NULL == outChar || NULL == inStruct)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    ucD7 = inStruct->direct;
    ucD6 = inStruct->reply;
    ucD5 = inStruct->follow;
    ucD40 = inStruct->funcode;

    ucCtrlCode |= ucD7 << 7;
    ucCtrlCode |= ucD6 << 6;
    ucCtrlCode |= ucD5 << 5;
    ucCtrlCode |= ucD40 & 0x1F;

    *outChar = ucCtrlCode;

    return E_D97_OK;
}

/*************************************************
Function:       trans_d97ctrl_char2struct
Description:    ���������������ֽ�ֵת��������ṹ���
                
Author:         liming 

Calls:          
Called By:      
Input:          S_D97_CTRL_CODE *inStruct

Output:         UINT8 *outChar
Return:      

Others:        ��ת�� trans_d97ctrl_struct2char
  
*************************************************/
eD97Err trans_d97ctrl_char2struct(UINT8 inChar,  S_D97_CTRL_CODE *outStruct)
{

    UINT8 ucCtrlCode = inChar;
    int func_code = 0;
    
    if(NULL == outStruct)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if((ucCtrlCode & 0x80) == 0)
    {
        outStruct->direct = E_D97_CTRL_DIR_M2S;
    }
    else
    {
        outStruct->direct = E_D97_CTRL_DIR_S2M;
    }

    if((ucCtrlCode & 0x40) == 0)
    {
        outStruct->reply = E_D97_CTRL_SR_OK;
    }
    else
    {
        outStruct->reply = E_D97_CTRL_SR_NO;
    }

    if((ucCtrlCode & 0x20) == 0)
    {
        outStruct->follow = E_D97_CTRL_FLW_NONE;
    }
    else
    {
        outStruct->follow = E_D97_CTRL_FLW_HAVE;
    }

    func_code = ucCtrlCode & 0x1F;

    switch(func_code)
    {
        case CTRL_CODE_RESERVE:
            outStruct->funcode = E_D97_CTRL_FNC_RESERVE;
            break;
            
        case CTRL_CODE_READ_DATA:
            outStruct->funcode = E_D97_CTRL_FNC_READ_DATA;
            break;
            
        case CTRL_CODE_READ_AFTERDATA:
            outStruct->funcode = E_D97_CTRL_FNC_READ_AFTERDATA;
            break;
            
        case CTRL_CODE_READ_DATA_RE:
            outStruct->funcode = E_D97_CTRL_FNC_READ_DATA_RE;
            break;
            
        case CTRL_CODE_WRITE_DATA:
            outStruct->funcode = E_D97_CTRL_FNC_WRITE_DATA;
            break;
            
        case CTRL_CODE_SYNC_TIME:
            outStruct->funcode = E_D97_CTRL_FNC_SYNC_TIME;
            break;
            
        case CTRL_CODE_WRITE_ADDR:
            outStruct->funcode = E_D97_CTRL_FNC_WRITE_ADDR;
            break;
            
        case CTRL_CODE_MODIFY_BAUD:
            outStruct->funcode = E_D97_CTRL_FNC_MODIFY_BAUD;
            break;
        case CTRL_CODE_MODIFY_PASSWORD:
            outStruct->funcode = E_D97_CTRL_FNC_MODIFY_PASSWORD;
            break;
            
        case CTRL_CODE_MAXDEMAND_CLEAR:
            outStruct->funcode = E_D97_CTRL_FNC_MAXDEMAND_CLEAR;
            break;
            
        default:
            return E_D97_ERROR_PARA_OUT_RANGE;
            
    }

    return E_D97_OK;
}


/* ����ķֽ�ת������ */
eD97Err trans_d97_format_X(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    UINT16 usNNNN = 0;
    char acTmp[2] = {0};
    char strOut[2] = {0};
    
#ifdef D97_DEBUG_ON
    int outLen = 0;
    UINT8 ucXX = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(E_D97_TRANS_U2F == flag)
    {
        
        usNNNN = *(UINT16*)user;
        
        sprintf(acTmp, "%02d", usNNNN);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 2);
            printf("\t(X) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        d97_remove_dot(2,acTmp, strOut);
        d97_str2bcd(strOut, frame, 2);
    }
    else
    {
        
        d97_bcd2str(frame, acTmp, 1);
        memcpy(user, (void*)acTmp, 2);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            int outLen = 0;
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data,"X\t=  %s\n", acTmp);
            UINT8 ucXX = atoi(acTmp);
            outLen = sprintf(g_d97_out_data + outLen," X\t=  %d\n", ucXX);
        }
        #endif
    }
    return E_D97_OK;
}


eD97Err trans_d97_format_XX(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    UINT16 usNNNN = 0;
    char acTmp[2] = {0};
    char strOut[2] = {0};    
#ifdef D97_DEBUG_ON
    UINT8 ucXX = 0;
    int outLen = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(E_D97_TRANS_U2F == flag)
    {
        usNNNN = *(UINT16*)user;
        sprintf(acTmp, "%02d", usNNNN);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 2);
            printf("\t(XX) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        d97_remove_dot(2,acTmp, strOut);
        d97_str2bcd(strOut, frame, 2);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 1);
        memcpy(user, (void*)acTmp, 2);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data,"XX\t=  %s\n", acTmp);
            ucXX = atoi(acTmp);
            outLen = sprintf(g_d97_out_data + outLen," XX\t=  %d\n", ucXX);
        }
        #endif
    }
    return E_D97_OK;
}

/*****************************************************************************
 �� �� ��  : trans_d97_format_XXX
 ��������  : XXX��ʽ���ݴ�����
 �������  : E_D97_TRANS_FLAG flag  
             void *user             
             void*frame             
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��11��20��
    ��    ��   : liming
    �޸�����   : �����ɺ���

*****************************************************************************/
eD97Err trans_d97_format_XXX(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    UINT16 usNNNN = 0;
    char acTmp[4] = {0};
    char strOut[4] = {0};
#ifdef D97_DEBUG_ON
    int outLen = 0;
    UINT16 usXXX = 0;
#endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(E_D97_TRANS_U2F == flag)
    {
        usNNNN = *(UINT16*)user;    
        sprintf(acTmp, "%04d", usNNNN);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 4);
            printf("\t(XXX) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        d97_remove_dot(4,acTmp, strOut);
        d97_str2bcd(strOut, frame, 4);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 2);
        memcpy(user, (void*)acTmp, 4);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {        
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data,"XXX\t=  %s\n", acTmp);
            usXXX = atoi(acTmp);
            outLen = sprintf(g_d97_out_data + outLen," XXX\t=  %d\n", usXXX);
        }
        #endif
        printf("acTmp = %s\n", acTmp);
    }
    return E_D97_OK;
}

eD97Err trans_d97_format_X_XXX(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    UINT16 usNNNN = 0;
    char acTmp[5] = {0};
    char strOut[4] = {0};
#ifdef D97_DEBUG_ON
    double d = 0.0;
    int outLen = 0;
#endif    

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(E_D97_TRANS_U2F == flag)
    {
        usNNNN = *(UINT16*)user;
        sprintf(acTmp, "%02d", usNNNN);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 5);
            printf("\t(X.XXX) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        d97_remove_dot(4,acTmp, strOut);
        d97_str2bcd(strOut, frame, 4);
    }
    else // frame to user
    {
        d97_bcd2str(frame, acTmp, 2);
        memcpy(user, (void*)acTmp, 4);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            d = d97_bcd_to_double(frame, 2, 3);
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data,"XXXX\t=  %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen," X.XXX\t=  %1.3f\n", d);
        }
        #endif
    }
    return E_D97_OK;
}


/*****************************************************************************
 �� �� ��  : trans_d97_format_XX_XX
 ��������  : like xx.xx format function
 �������  : E_D97_TRANS_FLAG flag  
             void *user             
             void*frame             
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��11��20��
    ��    ��   : liming
    �޸�����   : �����ɺ���

*****************************************************************************/
eD97Err trans_d97_format_XX_XX(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    UINT16 usNNNN = 0;
    char acTmp[5] = {0};
    char strOut[4] = {0};
#ifdef D97_DEBUG_ON
    double d = 0.0;
    int outLen = 0;
#endif    

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(E_D97_TRANS_U2F == flag)
    {
        usNNNN = *(UINT16*)user;
        sprintf(acTmp, "%02d", usNNNN);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 5);
            printf("\t(XX.XX) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        d97_remove_dot(4,acTmp, strOut);
        d97_str2bcd(strOut, frame, 4);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 2);
        memcpy(user, (void*)acTmp, 4);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            d = d97_bcd_to_double(frame, 2, 2);
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data,"XXXX\t=  %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen," XX.XX\t=  %2.2f\n", d);
        }
        #endif
    }
    return E_D97_OK;
}

eD97Err trans_d97_format_XX_XXXX(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    float fltXX_XXXX = 0.0;
    char acTmp[7] = {0};
    char strOut[7] = {0};
#ifdef D97_DEBUG_ON
    double d = 0.0;
    int outLen = 0;
#endif    

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if(E_D97_TRANS_U2F == flag)
    {
        fltXX_XXXX = *(float*)user;
        sprintf(acTmp, "%2.4f", fltXX_XXXX);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 7);
            printf("\t(XX.XXXX) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
    
        d97_remove_dot(6,acTmp, strOut);
        d97_str2bcd(strOut, frame, 6);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 3);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            d = d97_bcd_to_double(frame, 3, 4);
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data,"XXXXXX  =  %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen," XX.XXXX =  %2.4f\n", d);
        }
        #endif
    }
    return E_D97_OK;

}

eD97Err trans_d97_format_XXXXXX_XX(E_D97_TRANS_FLAG flag, void *user, void*frame)
{    
    float fltXXXXXX_XX = 0.0;
    char acTmp[9] = {0};
    char strOut[9] = {0};
    
 #ifdef D97_DEBUG_ON
    double d = 0.0;
    int outLen = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(E_D97_TRANS_U2F == flag)
    {    
        fltXXXXXX_XX = *(float*)user;
        sprintf(acTmp, "%6.2f", fltXXXXXX_XX);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 9);
            printf("\t(XXXXXX.XX) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
    
        d97_remove_dot(8,acTmp, strOut);
        d97_str2bcd(strOut, frame, 8);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 4);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            d = d97_bcd_to_double(frame, 4, 2);
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data,"XXXXXX.XX\t=  %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen," #double\t=  %6.2f\n", d);
        }
        #endif
    }
    return E_D97_OK;
}

/*YYMMDDhhmmss*/
eD97Err trans_d97_format_MMDDHHmm(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    S_D97_FMT_MMDDHHmm *pstUserData;
    char acTmp[9] = {0};
#ifdef D97_DEBUG_ON
    int outLen = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if(E_D97_TRANS_U2F == flag)
    {
        
        pstUserData = (S_D97_FMT_MMDDHHmm *)user;
        sprintf(acTmp, "%d%d%d%d", pstUserData->MM,pstUserData->DD,pstUserData->HH,pstUserData->mm);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 9);
            printf("\t(MMDDHHmm) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        
        d97_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else
    {    
        d97_bcd2str(frame, acTmp, 4);
        pstUserData = (S_D97_FMT_MMDDHHmm *)user;
        pstUserData->MM = d97_str_to_num(&acTmp[0],2);
        pstUserData->DD = d97_str_to_num(&acTmp[2],2);
        pstUserData->HH = d97_str_to_num(&acTmp[4],2);
        pstUserData->mm = d97_str_to_num(&acTmp[6],2);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data, "MMDDHHMM = %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen,
                            "time = %d��%d��%dʱ%d��\n",
                            pstUserData->MM,
                            pstUserData->DD,
                            pstUserData->HH,
                            pstUserData->mm);

        }
        #endif
    }
    return E_D97_OK;
}


eD97Err trans_d97_format_YYMMDDWW(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    S_D97_FMT_YYMMDDWW *pstUserData;
    char acTmp[9] = {0};
 #ifdef D97_DEBUG_ON
    int outLen = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if(E_D97_TRANS_U2F == flag)
    {
        pstUserData = (S_D97_FMT_YYMMDDWW *)user;
        sprintf(acTmp, "%d%d%d%d",pstUserData->YY, pstUserData->MM,pstUserData->DD,pstUserData->WW);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 9);
            printf("\t(YYMMDDWW) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        
        d97_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 4);
        pstUserData = (S_D97_FMT_YYMMDDWW *)user;
        pstUserData->YY = d97_str_to_num(&acTmp[0],2);
        pstUserData->MM = d97_str_to_num(&acTmp[2],2);
        pstUserData->DD = d97_str_to_num(&acTmp[4],2);
        pstUserData->WW = d97_str_to_num(&acTmp[6],2);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {        
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data, "YYMMDDWW = %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen,
                            "time = %d��%d��%d����%d\n",
                            pstUserData->YY,
                            pstUserData->MM,
                            pstUserData->DD,
                            pstUserData->WW);


        }
        #endif
    }
    return E_D97_OK;
}
eD97Err trans_d97_format_hhmmss(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    char acTmp[7] = {0};
    S_D97_FMT_hhmmss *pstUserData;
#ifdef D97_DEBUG_ON
    INT32 outLen = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if(E_D97_TRANS_U2F == flag)
    {
        pstUserData = (S_D97_FMT_hhmmss *)user;
        sprintf(acTmp, "%d%d%d", pstUserData->hh,pstUserData->mm,pstUserData->ss);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 7);
            printf("\t(hhmmss) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        
        d97_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else
    {    
        d97_bcd2str(frame, acTmp, 3);
        pstUserData = (S_D97_FMT_hhmmss *)user;
        pstUserData->hh = d97_str_to_num(&acTmp[0],2);
        pstUserData->mm = d97_str_to_num(&acTmp[2],2);
        pstUserData->ss = d97_str_to_num(&acTmp[4],2);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
                
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data, "hhmmss = %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen,
                            " time   = %dʱ%d��%d��\n",
                            pstUserData->hh,
                            pstUserData->mm,
                            pstUserData->ss);

        }
        #endif
    }
    return E_D97_OK;
}
// DDhh
eD97Err trans_d97_format_DDhh(E_D97_TRANS_FLAG flag, void *user, void*frame)
{

    char acTmp[5] = {0};
    S_D97_FMT_DDhh *pstUserData;
#ifdef D97_DEBUG_ON
    INT32 outLen = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if(E_D97_TRANS_U2F == flag)
    {    
        pstUserData = (S_D97_FMT_DDhh *)user;
        sprintf(acTmp, "%d%d", pstUserData->DD,pstUserData->hh);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 5);
            printf("\t(DDhh) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        
        d97_str2bcd(acTmp, (UINT8*)frame, 4);
    }
    else
    {    
        d97_bcd2str(frame, acTmp, 3);
        pstUserData = (S_D97_FMT_DDhh *)user;
        pstUserData->DD = d97_str_to_num(&acTmp[0],2);
        pstUserData->hh = d97_str_to_num(&acTmp[2],2);


        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data, "DDhh = %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen,
                            " time   = %d��%dʱ\n",
                            pstUserData->DD,
                            pstUserData->hh);

        }
        #endif
    }
    return E_D97_OK;
}

/*************************************************
Function:       trans_d97_format_stwd_meter
Description:     ���״̬����Ϣ �û�����֡����ת��
                
Author:         liming 

Calls:          
Called By:      
Input:          

Output:        
Return:      ������������ 0

Others:        flag ����ת������
  
*************************************************/
eD97Err trans_d97_format_stwd_meter(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    S_D97_STWD_METER *stUser;
    UINT8 ucFrame;

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
        
    ucFrame = *(UINT8*)frame;
    stUser = (S_D97_STWD_METER*)user;

    if(E_D97_TRANS_U2F == flag)
    {
        // user interface to Frame
        (stUser->dir_none == E_D97_ST_OK) ? (ucFrame &= 0xDF) : (ucFrame |= 0x20);
        (stUser->dir_have == E_D97_ST_OK) ? (ucFrame &= 0xEF) : (ucFrame |= 0x10);
        (stUser->cell     == E_D97_ST_OK) ? (ucFrame &= 0xFB) : (ucFrame |= 0x04);
        (stUser->demand   == E_D97_ST_OK) ? (ucFrame &= 0xFD) : (ucFrame |= 0x02);
        (stUser->read     == E_D97_ST_OK) ? (ucFrame &= 0xFE) : (ucFrame |= 0x01);
    }
    else // Frame to user interface 
    {
        stUser->dir_none  = (ucFrame & 0x20) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->dir_have  = (ucFrame & 0x10) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->cell       = (ucFrame & 0x04) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->demand    = (ucFrame & 0x02) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->read       = (ucFrame & 0x01) ? E_D97_ST_OK : E_D97_ST_NO;

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            sprintf(g_d97_out_data,
                    "����               = %s\n"
                    " �޹����ܷ���       = %s\n"
                    " �й����ܷ���       = %s\n"
                    " ��ص�ѹ           = %s\n"
                    " ����������㷽ʽ   = %s\n",
                    stUser->read ==   E_D97_ST_OK ? "�Զ�" : "�ֶ�",
                    stUser->dir_none == E_D97_ST_OK ? "����" : "����",
                    stUser->dir_have == E_D97_ST_OK ? "����" : "����",
                    stUser->cell  == E_D97_ST_OK ? "����" : "Ƿѹ", 
                    stUser->demand  == E_D97_ST_OK ? "����" : "����");
                            

        }
        #endif
    }
    
    return E_D97_OK;


}

/*************************************************
Function:       trans_d97_format_stwd_error
Description:     ����Ӧ��״̬����Ϣ �û�����֡����ת��
                
Author:         liming 

Calls:          
Called By:      
Input:          

Output:        
Return:      ������������ 0

Others:        flag ����ת������
  
*************************************************/
eD97Err trans_d97_format_stwd_error(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    S_D97_STWD_ERR *stUser;
    UINT8 ucFrame;
    
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    ucFrame = *(UINT8*)frame;
    stUser = (S_D97_STWD_ERR*)user;
    
    if(E_D97_TRANS_U2F == flag)
    {
        // user interface to Frame
        (stUser->rate_k   == E_D97_ST_OK) ? (ucFrame &= 0xBF) : (ucFrame |= 0x40);
        (stUser->day_q    == E_D97_ST_OK) ? (ucFrame &= 0xDF) : (ucFrame |= 0x20);
        (stUser->year_p   == E_D97_ST_OK) ? (ucFrame &= 0xEF) : (ucFrame |= 0x10);
        (stUser->password == E_D97_ST_OK) ? (ucFrame &= 0xFB) : (ucFrame |= 0x04);
        (stUser->rule_id  == E_D97_ST_OK) ? (ucFrame &= 0xFD) : (ucFrame |= 0x02);
        (stUser->data       == E_D97_ST_OK) ? (ucFrame &= 0xFE) : (ucFrame |= 0x01);
    }
    else // Frame to user interface 
    {
        stUser->rate_k      = (ucFrame & 0x40) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->day_q      = (ucFrame & 0x20) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->year_p      = (ucFrame & 0x10) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->password = (ucFrame & 0x04) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->rule_id  = (ucFrame & 0x02) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->data      = (ucFrame & 0x01) ? E_D97_ST_OK : E_D97_ST_NO;
    }
    
    return E_D97_OK;
}

/*************************************************
Function:       trans_d97_format_stwd_netwire
Description:     ��������״̬����Ϣ �û�����֡����ת��
                
Author:         liming 

Calls:          
Called By:      
Input:          

Output:        
Return:      ������������ 0

Others:        flag ����ת������
  
*************************************************/
eD97Err trans_d97_format_stwd_netwire(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    S_D97_STWD_NET_WIRE *stUser;
    UINT8 ucFrame;

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    ucFrame = *(UINT8*)frame;
    stUser = (S_D97_STWD_NET_WIRE*)user;
    
    if(E_D97_TRANS_U2F == flag)
    {
        // user interface to Frame
        (stUser->volt_C   == E_D97_ST_OK) ? (ucFrame &= 0xBF) : (ucFrame |= 0x40);
        (stUser->volt_B   == E_D97_ST_OK) ? (ucFrame &= 0xDF) : (ucFrame |= 0x20);
        (stUser->volt_A   == E_D97_ST_OK) ? (ucFrame &= 0xEF) : (ucFrame |= 0x10);
        (stUser->out_C       == E_D97_ST_OK) ? (ucFrame &= 0xFB) : (ucFrame |= 0x04);
        (stUser->out_B    == E_D97_ST_OK) ? (ucFrame &= 0xFD) : (ucFrame |= 0x02);
        (stUser->out_A       == E_D97_ST_OK) ? (ucFrame &= 0xFE) : (ucFrame |= 0x01);
    }
    else // Frame to user interface 
    {
        stUser->volt_C      = (ucFrame & 0x40) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->volt_B      = (ucFrame & 0x20) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->volt_A      = (ucFrame & 0x10) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->out_C    = (ucFrame & 0x04) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->out_B    = (ucFrame & 0x02) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->out_A      = (ucFrame & 0x01) ? E_D97_ST_OK : E_D97_ST_NO;

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            
            sprintf(g_d97_out_data,
                    " A���ѹ   = %s\n"
                    " B���ѹ   = %s\n"
                    " C���ѹ   = %s\n"
                    " A��ϵ�   = %s\n"
                    " B��ϵ�   = %s\n"
                    " C��ϵ�   = %s\n",
                    stUser->volt_A == E_D97_ST_OK ? "TRUE" : "FALSE",
                    stUser->volt_B == E_D97_ST_OK ? "TRUE" : "FALSE",
                    stUser->volt_C == E_D97_ST_OK ? "TRUE" : "FALSE",
                    stUser->out_A  == E_D97_ST_OK ? "TRUE" : "FALSE", 
                    stUser->out_B  == E_D97_ST_OK ? "TRUE" : "FALSE",
                    stUser->out_C  == E_D97_ST_OK ? "TRUE" : "FALSE");            

        }
        #endif
    }
    
    return E_D97_OK;

}

/*************************************************
Function:       trans_d97_format_stwd_weekday
Description:     ������״̬����Ϣ �û�����֡����ת��
                
Author:         liming 

Calls:          
Called By:      
Input:          

Output:        
Return:      ������������ 0

Others:        flag ����ת������
  
*************************************************/
eD97Err trans_d97_format_stwd_weekday(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    S_D97_STWD_WEEK_DAY *stUser;
    UINT8 ucFrame;
    
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    ucFrame = *(UINT8*)frame;
    stUser  = (S_D97_STWD_WEEK_DAY*)user;
    
    if(E_D97_TRANS_U2F == flag)
    {
        // user interface to Frame
        (stUser->day_6 == E_D97_ST_OK) ? (ucFrame &= 0xBF) : (ucFrame |= 0x40);
        (stUser->day_5 == E_D97_ST_OK) ? (ucFrame &= 0xDF) : (ucFrame |= 0x20);
        (stUser->day_4 == E_D97_ST_OK) ? (ucFrame &= 0xEF) : (ucFrame |= 0x10);
        (stUser->day_3 == E_D97_ST_OK) ? (ucFrame &= 0xFC) : (ucFrame |= 0x08);
        (stUser->day_2 == E_D97_ST_OK) ? (ucFrame &= 0xFB) : (ucFrame |= 0x04);
        (stUser->day_1 == E_D97_ST_OK) ? (ucFrame &= 0xFD) : (ucFrame |= 0x02);
        (stUser->day_7 == E_D97_ST_OK) ? (ucFrame &= 0xFE) : (ucFrame |= 0x01);
    }
    else // Frame to user interface 
    {
        stUser->day_6 = (ucFrame & 0x40) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->day_5 = (ucFrame & 0x20) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->day_4 = (ucFrame & 0x10) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->day_3 = (ucFrame & 0x08) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->day_2 = (ucFrame & 0x04) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->day_1 = (ucFrame & 0x02) ? E_D97_ST_OK : E_D97_ST_NO;
        stUser->day_7 = (ucFrame & 0x01) ? E_D97_ST_OK : E_D97_ST_NO;
    }
    
    return E_D97_OK;

}

eD97Err trans_d97_format_MMDDNN(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    S_D97_FMT_MMDDNN *pstUserData;
    char acTmp[7] = {0};
#ifdef D97_DEBUG_ON
    INT32 outLen = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if(E_D97_TRANS_U2F == flag)
    {    
        pstUserData = (S_D97_FMT_MMDDNN *)user;
        sprintf(acTmp, "%d%d%d", pstUserData->MM,pstUserData->DD,pstUserData->NN);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 7);
            printf("\t(MMDDNN) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        
        d97_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 3);
        pstUserData = (S_D97_FMT_MMDDNN *)user;
        pstUserData->MM = d97_str_to_num(&acTmp[0],2);
        pstUserData->DD = d97_str_to_num(&acTmp[2],2);
        pstUserData->NN = d97_str_to_num(&acTmp[4],2);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {    
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data, "hhmmss = %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen,
                            " time   = %d��%d�ձ��%d\n",
                            pstUserData->MM,
                            pstUserData->DD,
                            pstUserData->NN);

        }
        #endif
    }
    return E_D97_OK;
}
eD97Err trans_d97_format_MMDDhhmm(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    char acTmp[9] = {0};
    S_D97_FMT_MMDDHHmm *pstUserData;
#ifdef D97_DEBUG_ON
    INT32 outLen = 0;
#endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if(E_D97_TRANS_U2F == flag)
    {
        pstUserData = (S_D97_FMT_MMDDHHmm *)user;
        sprintf(acTmp, "%d%d%d%d", pstUserData->MM,pstUserData->DD,pstUserData->HH,pstUserData->mm);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 9);
            printf("\t(MMDDhhmm) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        
        d97_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else
    {    
        d97_bcd2str(frame, acTmp, 4);
        
        pstUserData = (S_D97_FMT_MMDDHHmm *)user;
        pstUserData->MM = d97_str_to_num(&acTmp[0],2);
        pstUserData->DD = d97_str_to_num(&acTmp[2],2);
        pstUserData->HH = d97_str_to_num(&acTmp[4],2);
        pstUserData->mm = d97_str_to_num(&acTmp[6],2);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {        
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data, "MMDDhhmm = %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen,
                            "time = %d��%d��%dʱ%d��\n",
                            pstUserData->MM,
                            pstUserData->DD,
                            pstUserData->HH,
                            pstUserData->mm);

        }
        #endif
    }
    return E_D97_OK;
}
eD97Err trans_d97_format_hhmmNN(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    S_D97_FMT_hhmmss *pstUserData;    
    char acTmp[7] = {0};

    #ifdef D97_DEBUG_ON
    int outLen = 0;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if(E_D97_TRANS_U2F == flag)
    {    
        pstUserData = (S_D97_FMT_hhmmss *)user;
        sprintf(acTmp, "%d%d%d", pstUserData->hh,pstUserData->mm,pstUserData->ss);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 7);
            printf("\t(hhmmNN) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        
        d97_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else
    {    
        d97_bcd2str(frame, acTmp, 3);
    
        pstUserData = (S_D97_FMT_hhmmss *)user;
        pstUserData->hh = d97_str_to_num(&acTmp[0],2);
        pstUserData->mm = d97_str_to_num(&acTmp[2],2);
        pstUserData->ss = d97_str_to_num(&acTmp[4],2);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {        
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data, "hhmmNN = %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen,
                            " time   = %dʱ%d��%d��\n",
                            pstUserData->hh,
                            pstUserData->mm,
                            pstUserData->ss);

        }
        #endif
    }
    return E_D97_OK;
}
eD97Err trans_d97_format_NN(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    char acTmp[3] = {0};
    char strOut[2] = {0};
    UINT16 usNNNN = 0;
    
    #ifdef D97_DEBUG_ON
    INT32 outLen = 0;
    UINT8 ucXX = 0;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(E_D97_TRANS_U2F == flag)
    {
        usNNNN = *(UINT16*)user;
    
        sprintf(acTmp, "%02d", usNNNN);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 2);
            printf("\t(NN) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
        d97_remove_dot(2,acTmp, strOut);
        d97_str2bcd(strOut, frame, 2);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 1);
        memcpy(user, (void*)acTmp, 2);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data,"NN\t=  %s\n", acTmp);
            ucXX = atoi(acTmp);
            outLen = sprintf(g_d97_out_data + outLen," NN\t=  %d\n", ucXX);
        }
        #endif
    }
    return E_D97_OK;
}

/*****************************************************************************
 �� �� ��  : trans_d97_format_NNNN
 ��������  : NNNN��ʽ��������
 �������  : E_D97_TRANS_FLAG flag  
             void *user             
             void*frame             
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��11��20��
    ��    ��   : liming
    �޸�����   : �����ɺ���

*****************************************************************************/
eD97Err trans_d97_format_NNNN(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    UINT16 usNNNN = 0;
    char acTmp[5] = {0};
    char strOut[5] = {0};
        
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(E_D97_TRANS_U2F == flag)
    {        
        usNNNN = *(UINT16*)user;        
        sprintf(acTmp, "%04d", usNNNN);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 5);
            printf("\t(NNNN) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
    
        d97_remove_dot(4,acTmp, strOut);
        d97_str2bcd(strOut, frame, 4);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 2);
        memcpy(user, (void*)acTmp, 4);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            sprintf(g_d97_out_data,"NNNN\t=  %s\n", acTmp);
        }
        #endif
    }
    return E_D97_OK;
}
/*****************************************************************************
 �� �� ��  : trans_d97_format_NNNNNN
 ��������  : ��ʽNNNNNN����ת������
 �������  : E_D97_TRANS_FLAG flag  
             void *user             
             void*frame             
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��11��20��
    ��    ��   : liming
    �޸�����   : �����ɺ���

*****************************************************************************/
eD97Err trans_d97_format_NNNNNN(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    char acTmp[7] = {0};
    UINT16 usNN_6 = 0;
    char strOut[6] = {0};
    
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
        
    if(E_D97_TRANS_U2F == flag)
    {        
        usNN_6 = *(UINT16*)user;
        sprintf(acTmp, "%06d", usNN_6);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 6);
            printf("\t(NNNNNN) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
    
        d97_remove_dot(6,acTmp, strOut);
        d97_str2bcd(strOut, frame, 6);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 3);
        memcpy(user, (void*)acTmp, 7);
        
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            sprintf(g_d97_out_data,"NNNNNN\t=  %s\n", acTmp);
        }
        #endif
    }
    return E_D97_OK;
}
eD97Err trans_d97_format_NNNNNNNN(E_D97_TRANS_FLAG flag, void *user, void*frame){return E_D97_OK;}

eD97Err trans_d97_format_NN_6(E_D97_TRANS_FLAG flag, void *user, void*frame)
{
    char acTmp[13] = {0};
    char strOut[12] = {0};
    UINT16 usNN_6 = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }
    
    if(E_D97_TRANS_U2F == flag)
    {    
        usNN_6 = *(UINT16*)user;
        sprintf(acTmp, "%06d", usNN_6);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 12);
            printf("\t(NNNNNNNNNNNN) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
    
        d97_remove_dot(12,acTmp, strOut);
        d97_str2bcd(strOut, frame, 12);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 6);
        memcpy(user, (void*)acTmp, 12);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            sprintf(g_d97_out_data,"NNNNNNNNNNNN\t=  %s\n", acTmp);
        }
        #endif
    }
    return E_D97_OK;
}


eD97Err trans_d97_format_NNNNNN_N(E_D97_TRANS_FLAG flag, void *user, void*frame)
{

    float fltNNNNNN_N = 0.0;
    char acTmp[9] = {0};
    char strOut[9] = {0};
    #ifdef D97_DEBUG_ON
    double d = 0.0;
    UINT32 outLen = 0;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D97_ERROR_PARA_POINTER_NULL;
    }

    if(E_D97_TRANS_U2F == flag)
    {        
        fltNNNNNN_N = *(float*)user;        
        sprintf(acTmp, "%6.1f", fltNNNNNN_N);

        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {
            memset((void*)acTmp, 0x00, 9);
            printf("\t(NNNNNN.N) << ");
            scanf("%s", (char*)&acTmp);
        }
        #endif
    
        d97_remove_dot(8,acTmp, strOut);
        d97_str2bcd(strOut, frame, 8);
    }
    else
    {
        d97_bcd2str(frame, acTmp, 4);
        #ifdef D97_DEBUG_ON
        if(D97_ON == g_d97_need_pack)
        {        
            d = d97_bcd_to_double(frame, 4, 1);
            memset((void *)g_d97_out_data, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_d97_out_data,"NNNNNN.N\t=  %s\n", acTmp);
            outLen = sprintf(g_d97_out_data + outLen," NNNNNN.N\t=  %6.1f\n", d);
        }
        #endif
    }
    return E_D97_OK;
}

/*****************************************************************************
 �� �� ��  : trans_d97_format_err_word
 ��������  : ת������״̬������
 �������  : E_D97_TRANS_FLAG flag  
             void *user             
             void*frame             
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��15��
    ��    ��   : liming
    �޸�����   : �����ɺ���

*****************************************************************************/
eD97Err trans_d97_format_err_word(E_D97_TRANS_FLAG flag, void *user, void *frame)
{
     S_D97_ERR *psErr;
     UINT8 ucErr = 0;
     //UINT8 *pucErr;

     if(!user || !frame)
     {
        return E_D97_ERROR_PARA_POINTER_NULL;
     }
     
     psErr = (S_D97_ERR *)user;

    if(flag == E_D97_TRANS_U2F)
    {
        // �û��ൽ֡��
        (psErr->bRateErr  == TRUE) ? (ucErr |= 0x40) : (ucErr &= 0xBF);
        (psErr->bDayErr   == TRUE) ? (ucErr |= 0x20) : (ucErr &= 0xDF);
        (psErr->bYearErr  == TRUE) ? (ucErr |= 0x10) : (ucErr &= 0xEF);
        (psErr->bPwdErr   == TRUE) ? (ucErr |= 0x04) : (ucErr &= 0xFB);
        (psErr->bErrID    == TRUE) ? (ucErr |= 0x02) : (ucErr &= 0xFD);
        (psErr->bErrData  == TRUE) ? (ucErr |= 0x01) : (ucErr &= 0xFE);

        //*pucErr = ucErr;
    }
    else
    {
        // ֡�ൽ�û���
        ucErr = *(UINT8 *)frame;
        psErr->bRateErr  = (ucErr & 0x40) ? TRUE : FALSE;
        psErr->bDayErr   = (ucErr & 0x20) ? TRUE : FALSE;
        psErr->bYearErr  = (ucErr & 0x10) ? TRUE : FALSE;
        psErr->bPwdErr   = (ucErr & 0x04) ? TRUE : FALSE;
        psErr->bErrID    = (ucErr & 0x02) ? TRUE : FALSE;
        psErr->bErrData  = (ucErr & 0x01) ? TRUE : FALSE;

        //��ʾ��Ϣ
        #ifdef D97_DEBUG_ON
        sprintf(g_d97_out_data, 
        "��������: \t%s\n"
        "��ʱ������: \t%s\n"
        "��ʱ������: \t%s\n"
        "�����/δ��Ȩ: \t%s\n"
        "������������: \t%s\n"
        "�Ƿ�����: \t%s\n", 
        psErr->bRateErr  == TRUE ? "��" : "��",
        psErr->bDayErr   == TRUE ? "��" : "��",
        psErr->bYearErr  == TRUE ? "��" : "��",
        psErr->bPwdErr   == TRUE ? "��" : "��",
        psErr->bErrID  == TRUE ? "��" : "��",
        psErr->bErrData == TRUE ? "��" : "��");
        #endif

    }
    
    
    return E_D97_OK;


}