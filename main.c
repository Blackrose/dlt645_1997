#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include "dlt645_api_97.h"

//int g_num = 0;

void debug_switch(int show)
{
	 g_d97_need_pack = show;
}

void show_packet(int buflen, char* buf)
{	 
    printf("FE FE FE ");
	int i;
	for(i = 0; i < buflen;i++)
	{
		printf("%02X ", (unsigned char)buf[i]);
	} 
}

void unpack_d97_frame_test(int argc, char*argv[]);
int pack_d97_frame_test(unsigned short usRulerID);
/* ���Ժ��� */
typedef struct
{
	E_D97_DATA_FORMAT format;
	char* fmtName;	
}d97_format_test;

typedef struct
{		F_D97_RULER_TRANS pFunc;
		char* 	 funcName;
}d97_func_test;



char helpinfo[] = 
	{
		"dlt645 2097 ruler anlaysis program.\n"
		"usage:d97 [-i]\n"
		"--help,-h: help info\n"
		"--info,-i, get ruler infor\n"
		"--all, show all valid ruler infor\n"
	};



int d97_show_ruler_info(unsigned short rulerID);

int main(int argc, char *argv[])
{

	int opt = 0;
	int longIndex = 0;
	int ret = 0;
	int num = 0;
	char *strOpt = "hvi:p:u:";
	struct option longOpts[] = 
	{
		{"version",		no_argument, 		NULL, 	'v'},
		{"info",		required_argument,	NULL,	'i'},
		{"pack",		required_argument,	NULL,	'p'},
		{"unpack",		required_argument,	NULL,	'u'},
		{"help",		no_argument,		NULL,	'h'},
		{"all",			no_argument,		NULL,	0},
		{NULL,			no_argument,		NULL,	0}
	};

	unsigned short usRulerID = 0;
	
	while((opt = getopt_long(argc, argv,strOpt, longOpts, &longIndex)) != -1)
	{
		switch(opt)
		{
			case 'v':
				printf("**************************************\n");
				printf("     name :\tDL/T645 1997 tools\n");
				printf("  version :\t1.0\n");
				printf("   author :\tliming\n");
				printf("    email :\tvliming@126.com\n");
				printf("**************************************\n");
				break;
				
			case 'i':
				usRulerID = (unsigned short)strtol(optarg,NULL,16); // ���ַ���ʮ��������ת��Ϊ����
				ret = d97_show_ruler_info(usRulerID);
				if(ret != E_D97_OK)
				{
					printf("�����ڵĹ�Լ����\n\n");
				}
				break;

			case 'p':
				usRulerID = (unsigned short)strtol(optarg,NULL,16); // ���ַ���ʮ��������ת��Ϊ����
				ret = pack_d97_frame_test(usRulerID);
				break;
	
			case 'h':
				printf("%s\n",helpinfo);
				break;
								
			case 'u':
				unpack_d97_frame_test(argc, argv);
				break;
				

			case 0:
				if(!strcmp("all", longOpts[longIndex].name))
				{	
					//num = show_all_valid_ruler_info();
					printf("һ��%d����Ч�Ĺ�Լ\n", num);
				}
				break;
				
			default:
				printf("Your command line parameters are not right!\n");  
				return -1;
		}	
		
	}

	return 0;
}


int pack_d97_frame_test(unsigned short usRulerID)
{
	printf("0x%02X\n", usRulerID);
	int ret = 0;
	int num = 1;

	S_D97_RULER_INFOR info = {0};
	
	ret = get_d97_ruler_info(usRulerID,&info);
	if(ret != E_D97_OK)
	{
		printf("�����ڵĹ��Լ����\n\n");
		return ret;
	}

	unsigned char ucCtrl = 0;
	S_D97_CTRL_CODE stCtrl = {0};
	int dir = 0;
	
	char addr[64] = {0}; // ��ַ
	S_D97_PACK_FRAME pack_frame = {0};
	int length = 0;
	char buffer[256] = {0};
	char data[256] = {0};
	char user[256] = {0};
	F_D97_RULER_TRANS func = NULL;
	printf("\n-------------------------------------------------------------------");
	printf("\n%d - �����ַ(С�ڵ���12λ):\n\n\taddress <<  ", num++);
	scanf("%s",addr);

	// ����
	printf("\n%d - ���䷽��:   ��վ����վ(0)  ��վ����վ(1)\n\n\tdirect << ", num++);
	scanf("%d", &dir);
	if(dir != 0 && dir != 1)
	{
		printf("\n�������\n");
		return;
	}

	stCtrl.direct = (E_D97_CTRL_DIR)dir;

	// ��վ����վ
	if(E_D97_CTRL_DIR_M2S == dir)
	{
		if(info.rdwr == E_D97_RDWR_READ_ONLY)
		{
			stCtrl.funcode = E_D97_CTRL_FNC_READ_DATA;
			pack_frame.data_len = 2;
			pack_frame.data = NULL;
		}
		else if(info.rdwr == E_D97_RDWR_READ_WRITE)
		{
			//������
			printf("\n%d - ������:  ������(0) д����(1)\n operate << ",num++);
			scanf("%d", &dir);
			if(dir == 0)
			{
				stCtrl.funcode = E_D97_CTRL_FNC_READ_DATA;
				pack_frame.data_len = 2;
				pack_frame.data = NULL;
			}
			else if(1 == dir)
			{
				debug_switch(D97_ON);
				stCtrl.funcode = E_D97_CTRL_FNC_WRITE_DATA;
				pack_frame.data_len = info.length+ 2;
				func = info.func;
				func(E_D97_TRANS_U2F,user, data);
				pack_frame.data = data;
				debug_switch(D97_OFF);
			}
			else
			{
				printf("\n�������\n");
				return;
			}
		}
	}
	else	// ��վ����վ
	{
		printf("\n%d - ����������:\n\n",num++);
		// ��ȷ��Ӧ��
		func = info.func;
		debug_switch(D97_ON);
		func(E_D97_TRANS_U2F,user, data);
		debug_switch(D97_OFF);
		stCtrl.funcode = E_D97_CTRL_FNC_READ_DATA;
		pack_frame.data = data;
		pack_frame.data_len = info.length+ 2;
		// �쳣��Ӧ��
	}

	ret = trans_d97ctrl_struct2char(&ucCtrl, &stCtrl); //���ṹ��װ���ֽ�
	
	if(ret != E_D97_OK)
	{
		printf("\ntrans_d97ctrl_struct2char failed %d\n", ret);
		return;
	}
	
	pack_frame.ruler_id = usRulerID;
	pack_frame.ctrl_code = ucCtrl;
	pack_frame.address = addr;
	
	ret = pack_d97_frame_by_data(&pack_frame, buffer, &length);
	if(ret != E_D97_OK)
	{
		printf("\npack frame failed %d\n", ret);
		return;
	}

	printf("\n%d - ruler infor:\n\n", num++);
	d97_show_ruler_info(usRulerID);
	printf(" ctrl\t=  0x%02X\n", ucCtrl);
	printf(" addr\t=  %s\n", addr);

	
	//��ʾ֡����
	printf("\n%d - frame:\n\n\t", num++);
	show_packet(length, buffer);
	printf("\n\n-------------------------------------------------------------------\n");
	return E_D97_OK;
}


int d97_show_ruler_info(unsigned short rulerID)
{
	int ret = 0;
	unsigned char ucDi3 = 0;
	unsigned char ucDi2 = 0;
	unsigned char ucDi1 = 0;
	unsigned char ucDi0 = 0;

	ucDi0 = rulerID & 0xF;
	ucDi1 = (rulerID >> 4) & 0xF;
	ucDi2 = (rulerID >> 8) & 0xF;
	ucDi3 = (rulerID >> 12) & 0xF;
	int i;



	d97_format_test aFormat[] = 
	{	
		{E_D97_FORMAT_UNKOWN, "E_D97_FORMAT_UNKOWN"},
		{E_D97_FORMAT_MMDDHHmm, "E_D97_FORMAT_MMDDHHmm"},
		{E_D97_FORMAT_YYMMDDWW, "E_D97_FORMAT_YYMMDDWW"},
		{E_D97_FORMAT_hhmmss, "E_D97_FORMAT_hhmmss"},
		{E_D97_FORMAT_DDhh, "E_D97_FORMAT_DDhh"},
		{E_D97_FORMAT_MMDDNN, "E_D97_FORMAT_MMDDNN"},
		{E_D97_FORMAT_MMDDhhmm, "E_D97_FORMAT_MMDDhhmm"},
		{E_D97_FORMAT_hhmmNN, "E_D97_FORMAT_hhmmNN"},
		{E_D97_FORMAT_NN_1, "E_D97_FORMAT_NN"},
		{E_D97_FORMAT_NN_2, "E_D97_FORMAT_NNNN"},
		{E_D97_FORMAT_NN_3, "E_D97_FORMAT_NNNNNN"},
		{E_D97_FORMAT_NN_4, "E_D97_FORMAT_NN_4"},
		{E_D97_FORMAT_NN_5, "E_D97_FORMAT_NN_5"},
		{E_D97_FORMAT_NN_6, "E_D97_FORMAT_NN_6"},
		{E_D97_FORMAT_NN_7, "E_D97_FORMAT_NN_7"},
		{E_D97_FORMAT_NN_8, "E_D97_FORMAT_NN_8"},
		{E_D97_FORMAT_NN_9, "E_D97_FORMAT_NN_9"},
		{E_D97_FORMAT_NNNNNNN_N,"E_D97_FORMAT_NNNNNNN_N"},
		{E_D97_FORMAT_X, "E_D97_FORMAT_X"},
		{E_D97_FORMAT_XX, "E_D97_FORMAT_XX"},
		{E_D97_FORMAT_XXX, "E_D97_FORMAT_XXX"},
		{E_D97_FORMAT_X_XXX, "E_D97_FORMAT_X_XXX"},
		{E_D97_FORMAT_XX_XX, "E_D97_FORMAT_XX_XX"},
		{E_D97_FORMAT_XX_XXXX, "E_D97_FORMAT_XX_XXXX"},
		{E_D97_FORMAT_XXXXXX_XX, "E_D97_FORMAT_XXXXXX_XX"},
		{E_D97_FORMAT_STWD_METER_RUN, "E_D97_FORMAT_STWD_METER_RUN"},
		{E_D97_FORMAT_STWD_NETWIRE_RUN, "E_D97_FORMAT_STWD_NETWIRE_RUN"},
		{E_D97_FORMAT_STWD_WEEK_DAY, "E_D97_FORMAT_STWD_WEEK_DAY"}
	};

	d97_func_test aFunc[] = 
	{
		{NULL, "unkown function"},
		{trans_d97_format_X, "trans_d97_format_X"},
		{trans_d97_format_XX, "trans_d97_format_XX"},
		{trans_d97_format_XXX, "trans_d97_format_XXX"},
		{trans_d97_format_X_XXX, "trans_d97_format_X_XXX"},
		{trans_d97_format_MMDDHHmm, "trans_d97_format_MMDDHHmm"},
		{trans_d97_format_YYMMDDWW, "trans_d97_format_YYMMDDWW"},
		{trans_d97_format_hhmmss, "trans_d97_format_hhmmss"},
		{trans_d97_format_DDhh, "trans_d97_format_DDhh"},
		{trans_d97_format_XXXXXX_XX, "trans_d97_format_XXXXXX_XX"},
		{trans_d97_format_XX_XXXX, "trans_d97_format_XX_XXXX"},
		{trans_d97_format_XX_XX, "trans_d97_format_XX_XX"},
		{trans_d97_format_stwd_meter, "trans_d97_format_stwd_meter"},
		{trans_d97_format_stwd_netwire, "trans_d97_format_stwd_netwire"},
		{trans_d97_format_stwd_weekday, "trans_d97_format_stwd_weekday"},
		{trans_d97_format_stwd_error, "trans_d97_format_stwd_error"},
		{trans_d97_format_MMDDNN, "trans_d97_format_MMDDNN"},
		{trans_d97_format_MMDDhhmm, "trans_d97_format_MMDDhhmm"},
		{trans_d97_format_hhmmNN, "trans_d97_format_hhmmNN"},
		{trans_d97_format_NN, "trans_d97_format_NN"},
		{trans_d97_format_NNNN, "trans_d97_format_NNNN"},
		{trans_d97_format_NNNNNN, "trans_d97_format_NNNNNNNN"},
		{trans_d97_format_NN_6, "trans_d97_format_NN_6"},
		{trans_d97_format_NNNNNN_N, "trans_d97_format_NNNNNN_N"}
			
	};
	
	S_D97_RULER_INFOR strRulerInfo = {0};
	
	ret = get_d97_ruler_info(rulerID,&strRulerInfo);
	if(ret != E_D97_OK)
	{
		return ret;
	}

	//printf("\n---------------------[%d]---------------------\n\n", ++g_num);					
	printf(" id\t=  0x%04X :", rulerID);
	printf(" [%X][%X][%X][%X]\n", ucDi3, ucDi2, ucDi1, ucDi0);

	//strRulerInfo
	if(E_D97_RULER_UNKNOWN != strRulerInfo.type)
		{
			printf(" type\t=  %d\n", strRulerInfo.type);
		
			
			// data length			
			printf(" dLen\t=  %d\n", strRulerInfo.length);

			// read or write
			if(strRulerInfo.rdwr == E_D97_RDWR_READ_ONLY)
			{
				printf(" rdwr\t=  read only\n");
			}
			else if(strRulerInfo.rdwr == E_D97_RDWR_READ_WRITE)
			{
				printf(" rdwr\t=  read write\n");
			}
			else
			{
				printf(" rdwr\t=  write only\n");
			}

			//format
			printf(" format\t=");
			for(i = 0; i < E_D97_FORMAT_MAX; i++)
			{
				if(strRulerInfo.format == aFormat[i].format)
				{
					printf("  %s\n", aFormat[i].fmtName);
					break;
				}
			}
		
			printf(" func\t=");
			for(i = 0; i < E_D97_FORMAT_MAX; i++)
			{
				if(strRulerInfo.func == aFunc[i].pFunc)
				{
					printf("  %s\n", aFunc[i].funcName);
					break;
				}
			}
		
			
			printf(" name\t=  %s\n\n", strRulerInfo.name);
			
		}
	
	return E_D97_OK;
}

void unpack_d97_frame_test(int argc, char*argv[])
{
	int i,j = 0;
	int ret = 0;
	char buf[512] = {0};
	int len = 0;
	S_D97_UNPACK stUnPack = {0};
	
	//printf("buffer = ");
	for(i = 2; i < argc; i++)
	{
		//printf("%s", argv[i]);
		buf[j++] = strtol(argv[i], NULL,16);
	}

	len = j;
	debug_switch(D97_ON);
	ret = unpack_d97_frame(buf, len, &stUnPack);
	debug_switch(D97_OFF);
	if(ret != E_D97_OK)
	{
		printf("\n  ��������! (error = %d : ",ret);
		switch(ret)
		{
			case E_D97_ERROR_FRAME_UNCOMP:
				printf("��������֡����)\n");
				break;

			case E_D97_ERROR_FRAME_0x68:
				printf("��ʼ�� 0x68 ��λ�ò���)\n");
				break;
				
			case E_D97_ERROR_FRAME_CHECK_SUM:
				printf("����֡����Ͳ���)\n");
				break;
				
			case E_D97_ERROR_FRAME_END_0x16:
				printf("����֡������ 0x16 ����)\n");
				break;
			default:
				break;
		}
		
		printf("\n");
		return;
	}

	// ��ʾ���
	printf("/------------------unpack result-------------------\\\n");
	printf("\n<1> - ruler info:\n");
	d97_show_ruler_info(stUnPack.ruler_id);
	printf("\n<2> - frame info:\n");
	printf(" ctrl_code\t=  0x%02X\n", stUnPack.ctrl_c);
	printf(" 0xFE_num\t=  %d\n", stUnPack.lead_num);
	printf(" data_len\t=  %d\n", stUnPack.data_len);
	printf(" frame_len\t=  %d\n", stUnPack.frame_len);
	printf(" address\t=  %s\n", stUnPack.address);
    if(stUnPack.flag  == E_D97_UNPD_FLG_ERROR_OK)
    {
        printf("�쳣�Ļش�\n");
    }
    else
    {
         printf("�����Ļش�\n");
    }

    printf("\n<3> - data fields:\n");
    printf(" %s\n", g_d97_out_data);
	printf("\n<4> - remark:\n\n\t");
	if(stUnPack.ctrl_s.direct == E_D97_CTRL_DIR_M2S)
	{
		printf("��վ����վ��");
		if(E_D97_CTRL_FNC_READ_DATA == stUnPack.ctrl_s.funcode)
		{
			printf("������");
		}
		else if(E_D97_CTRL_FNC_WRITE_DATA == stUnPack.ctrl_s.funcode)
		{
			printf("д����");
		}
		printf("����(dlt645 97֡)\n\n");
	}
	else
	{		
		printf("��վ����վ��");
		if(E_D97_CTRL_FNC_READ_DATA == stUnPack.ctrl_s.funcode)
		{
			printf("�Զ������������Ӧ");
		}
		else if(E_D97_CTRL_FNC_WRITE_DATA == stUnPack.ctrl_s.funcode)
		{
			printf("��д���ݳɹ�����Ӧ");
		}
		printf("(dlt645 97֡)\n\n");
	}

	printf("\\------------------unpack result-------------------/\n");
}



