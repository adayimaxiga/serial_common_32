����ϵͳ����λ��������ֲ������

0.��cube�����ô��ڣ�pc���ں�judgement���ڣ�����Ҫ��������
	
	
	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	
	�������ͺͽ���dma����ͨģʽ��Ĭ��״̬����


1.�������ļ������������£�

	task_communicate.c
	task_communicate.h
	
	bsp_usart.c
	bsp_usart.h

	data_fifo.c
	dada_fifo.h
	
	protocol.c
	protocol.h
	
2.��freertos.c�ﶨ����������
	
	osThreadId pc_unpack_task_t;
	osThreadId judge_unpack_task_t;
	
3.��freertos.c�ļ������ʼ���г�ʼ������(�����ʹ��judgement�������ﲻ��ʼ��pc����pc�Ĵ���Ҳ��pc�����г�ʼ��������ʼ��pc������ʹ�ܴ���)��

	osThreadDef(pcunpackTask, pc_unpack_task, osPriorityNormal, 0, 512);
	pc_unpack_task_t = osThreadCreate(osThread(pcunpackTask), NULL);
	
	osThreadDef(unpackTask, judge_unpack_task, osPriorityNormal, 0, 512);
	judge_unpack_task_t = osThreadCreate(osThread(unpackTask), NULL);
	
4.��freertos.c�ļ����ͷ�ļ� "task_communicate.h"

5.��task_communicate.c�ļ��ĵ�һ������communicate_param_init()�зֱ����ô��ڼ�dma��������Ҫ�������¼�������������Ϊʾ��

	judgement_usart.USARTx = UART5;
	
	judgement_usart.rx_DMAx = DMA1;
	judgement_usart.rx_Stream = LL_DMA_STREAM_0;
	judgement_usart.rx_dma_stream=DMA1_Stream0;

	judgement_usart.tx_DMAx = DMA1;
	judgement_usart.tx_Stream = LL_DMA_STREAM_7;
	judgement_usart.tx_dma_stream=DMA1_Stream7;

6.��main.c�����ͷ�ļ� "task_communicate.h"�����ڳ�ʼ���׶ε���communicate_param_init()


7.��dma�����ж������HT��TC��־λ��ʾ������

	LL_DMA_ClearFlag_HT0(DMA1);
	LL_DMA_ClearFlag_TC0(DMA1);
	
8.��dma�����ж������TC��־λ����ʧ��dma��������������Ӧ���ڷ������flag��ʾ������

	LL_DMA_ClearFlag_TC7(DMA1);
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_7);
	judgement_usart.tx_finish_flag=1;

9.�ڶ�Ӧ�Ĵ��ڿ����ж��е���bsp_usart.c�ļ��е�usart_inturrupt_processed(usart_param_struct usart_param)����������Ϊ��Ӧ��judgement��pc����
	ʾ��
	usart_inturrupt_processed(judgement_usart);

10.��stm32f4xx_it.c�ļ��а���ͷ�ļ���bsp_usart.h����������ⲿ������

	extern usart_param_struct judgement_usart;
	extern usart_param_struct computer_usart;

congratulations��	���ˣ������Ѿ���ɣ�����������Ѿ����Խ��շ������ݡ�

����ϵͳ����������task_communicate.c�к���judgement_data_handler(uint8_t *p_frame)�����յ���������judge_rece_mesg�ṹ����

%%%%%%%%%%%%%%%%%%%---�������ݵķ���---%%%%%%%%%%%%%%%%%%%%%%%%%%%%


�������ݷ���Ϊ������һ���ṹ�壬�������������ݣ������ݴ��data_packet_pack������������һ���ź���osSignalSet

�Բ���ϵͳΪ����
����ϵͳ�Զ������ݽṹ�嶨��Ϊ��
typedef __packed struct
{
  float data1;
  float data2;
  float data3;
	uint8_t data4;
} client_show_data_t;

���ʹ��룺
		client_show_data.data1=1;
		client_show_data.data2=2;
		client_show_data.data3=3;
		client_show_data.data4=4;
		data_packet_pack(ROBOT_TO_CLIENT_ID, (uint8_t *)&client_show_data,sizeof(client_show_data), DN_REG_ID);
		osSignalSet(judge_unpack_task_t, UART_TX_SIGNAL);

Attention������
��δ���ռ�ýϴ��ջ�ռ䣬�����Ҫ�����ô˺�������������200�������ڴ�ռ�

����data_packet_pack�����ĸ�������

void data_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof)

��һ��ΪЭ��������룬�ڶ���Ϊ������ʼ��ַ��������Ϊ���ݳ��ȣ����ĸ�Ϊ���ͷ�������λ�����ͻ�������λ�����ͣ�


�����ֻ�������ϵͳ�����ݣ�ֱ��ʹ��ʾ�����뼴�ɡ�



