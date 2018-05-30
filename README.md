裁判系统及上位机串口移植方法：

0.在cube中配置串口（pc串口和judgement串口），主要参数如下
	
	
	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	
	开启发送和接收dma，普通模式，默认状态即可


1.把以下文件拷贝到工程下：

	task_communicate.c
	task_communicate.h
	
	bsp_usart.c
	bsp_usart.h

	data_fifo.c
	dada_fifo.h
	
	protocol.c
	protocol.h
	
2.在freertos.c里定义任务句柄：
	
	osThreadId pc_unpack_task_t;
	osThreadId judge_unpack_task_t;
	
3.在freertos.c文件任务初始化中初始化任务(如果仅使用judgement可在这里不初始化pc任务，pc的串口也在pc任务中初始化，不初始化pc任务则不使能串口)：

	osThreadDef(pcunpackTask, pc_unpack_task, osPriorityNormal, 0, 512);
	pc_unpack_task_t = osThreadCreate(osThread(pcunpackTask), NULL);
	
	osThreadDef(unpackTask, judge_unpack_task, osPriorityNormal, 0, 512);
	judge_unpack_task_t = osThreadCreate(osThread(unpackTask), NULL);
	
4.在freertos.c文件添加头文件 "task_communicate.h"

5.在task_communicate.c文件的第一个函数communicate_param_init()中分别配置串口及dma参数，主要配置以下几个参数，以下为示例

	judgement_usart.USARTx = UART5;
	
	judgement_usart.rx_DMAx = DMA1;
	judgement_usart.rx_Stream = LL_DMA_STREAM_0;
	judgement_usart.rx_dma_stream=DMA1_Stream0;

	judgement_usart.tx_DMAx = DMA1;
	judgement_usart.tx_Stream = LL_DMA_STREAM_7;
	judgement_usart.tx_dma_stream=DMA1_Stream7;

6.在main.c里添加头文件 "task_communicate.h"，并在初始化阶段调用communicate_param_init()


7.在dma接收中断中清除HT和TC标志位，示例如下

	LL_DMA_ClearFlag_HT0(DMA1);
	LL_DMA_ClearFlag_TC0(DMA1);
	
8.在dma发送中断中清除TC标志位，并失能dma数据流，并立对应串口发送完成flag，示例如下

	LL_DMA_ClearFlag_TC7(DMA1);
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_7);
	judgement_usart.tx_finish_flag=1;

9.在对应的串口空闲中断中调用bsp_usart.c文件中的usart_inturrupt_processed(usart_param_struct usart_param)函数，参数为对应的judgement和pc串口
	示例
	usart_inturrupt_processed(judgement_usart);

10.在stm32f4xx_it.c文件中包含头文件“bsp_usart.h”，并添加外部变量：

	extern usart_param_struct judgement_usart;
	extern usart_param_struct computer_usart;

congratulations！	至此，配置已经完成，正常情况下已经可以接收发送数据。

裁判系统接收数据在task_communicate.c中函数judgement_data_handler(uint8_t *p_frame)，接收到的数据在judge_rece_mesg结构体中

%%%%%%%%%%%%%%%%%%%---发送数据的方法---%%%%%%%%%%%%%%%%%%%%%%%%%%%%


发送数据方法为：定义一个结构体，在里面填上数据，将数据打包data_packet_pack，给发送任务一个信号量osSignalSet

以裁判系统为例：
裁判系统自定义数据结构体定义为：
typedef __packed struct
{
  float data1;
  float data2;
  float data3;
	uint8_t data4;
} client_show_data_t;

发送代码：
		client_show_data.data1=1;
		client_show_data.data2=2;
		client_show_data.data3=3;
		client_show_data.data4=4;
		data_packet_pack(ROBOT_TO_CLIENT_ID, (uint8_t *)&client_show_data,sizeof(client_show_data), DN_REG_ID);
		osSignalSet(judge_unpack_task_t, UART_TX_SIGNAL);

Attention！！！
这段代码占用较大的栈空间，因此需要给调用此函数的任务至少200的任务内存空间

来看data_packet_pack函数四个参数：

void data_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof)

第一个为协议的命令码，第二个为数据起始地址，第三个为数据长度，第四个为发送方向（向上位机发送还是向下位机发送）


如果你只想给裁判系统发数据，直接使用示例代码即可。



