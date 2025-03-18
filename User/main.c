/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/05/31
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
TCP Client example, demonstrating that TCP Client connects
to the server and receives data and then sends it back.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
 */
#include "string.h"
#include "eth_driver.h"
#include <mbedtls/debug.h>
#include <mbedtls/platform.h>
#include <mbedtls/config.h>
#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/memory_buffer_alloc.h>
#define KEEPALIVE_ENABLE         1                          //Enable keep alive function
#include "cJSON.h"
// 定义接收缓冲区及标志位
#define RX_BUF_SIZE 128
volatile uint8_t rx_buf[RX_BUF_SIZE];
volatile uint16_t rx_cnt = 0;
volatile uint8_t rx_flag = 0; // 接收完成标志
u8 MACAddr[6];                                              //MAC address
u8 IPAddr[4]   = {0, 0, 0, 0};                    //IP address
u8 GWIPAddr[4] = {0, 0, 0, 0};                    //Gateway IP address
u8 IPMask[4]   = {0, 0, 0, 0};                    //subnet mask
u8 DESIP[4] = {106, 14, 246, 150};
u16 desport = 443;                                         //destination port
u16 srcport = 1000;                                         //source port
#define FALSE 0
#define TRUE  1
u8 SocketId;
u8 socket[WCHNET_MAX_SOCKET_NUM];                           //Save the currently connected socket
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];      //socket receive buffer
#define STATIC_MEM_POOL_SIZE (20 * 1024) // 建议至少18KB
static unsigned char static_mem_pool[STATIC_MEM_POOL_SIZE];
#define SERVER_NAME "api.siliconflow.cn"
#define API_KEY    "sk-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define MODEL_NAME "Pro/deepseek-ai/DeepSeek-R1-Distill-Qwen-1.5B"
const char* pers = "ssl_client1";
u8 bSendSocket = FALSE;
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_ssl_context ssl;
mbedtls_ssl_config conf;
// mbedtls_x509_crt cacert;

// 硅基流动CA证书
// static const char ca_cert[] = 
// "-----BEGIN CERTIFICATE-----\r\n"
// "MIIEMjCCAxqgAwIBAgIBATANBgkqhkiG9w0BAQUFADB7MQswCQYDVQQGEwJHQjEb\r\n"
// "MBkGA1UECAwSR3JlYXRlciBNYW5jaGVzdGVyMRAwDgYDVQQHDAdTYWxmb3JkMRow\r\n"
// "GAYDVQQKDBFDb21vZG8gQ0EgTGltaXRlZDEhMB8GA1UEAwwYQUFBIENlcnRpZmlj\r\n"
// "YXRlIFNlcnZpY2VzMB4XDTA0MDEwMTAwMDAwMFoXDTI4MTIzMTIzNTk1OVowezEL\r\n"
// "MAkGA1UEBhMCR0IxGzAZBgNVBAgMEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UE\r\n"
// "BwwHU2FsZm9yZDEaMBgGA1UECgwRQ29tb2RvIENBIExpbWl0ZWQxITAfBgNVBAMM\r\n"
// "GEFBQSBDZXJ0aWZpY2F0ZSBTZXJ2aWNlczCCASIwDQYJKoZIhvcNAQEBBQADggEP\r\n"
// "ADCCAQoCggEBAL5AnfRu4ep2hxxNRUSOvkbIgwadwSr+GB+O5AL686tdUIoWMQua\r\n"
// "BtDFcCLNSS1UY8y2bmhGC1Pqy0wkwLxyTurxFa70VJoSCsN6sjNg4tqJVfMiWPPe\r\n"
// "3M/vg4aijJRPn2jymJBGhCfHdr/jzDUsi14HZGWCwEiwqJH5YZ92IFCokcdmtet4\r\n"
// "YgNW8IoaE+oxox6gmf049vYnMlhvB/VruPsUK6+3qszWY19zjNoFmag4qMsXeDZR\r\n"
// "rOme9Hg6jc8P2ULimAyrL58OAd7vn5lJ8S3frHRNG5i1R8XlKdH5kBjHYpy+g8cm\r\n"
// "ez6KJcfA3Z3mNWgQIJ2P2N7Sw4ScDV7oL8kCAwEAAaOBwDCBvTAdBgNVHQ4EFgQU\r\n"
// "oBEKIz6W8Qfs4q8p74Klf9AwpLQwDgYDVR0PAQH/BAQDAgEGMA8GA1UdEwEB/wQF\r\n"
// "MAMBAf8wewYDVR0fBHQwcjA4oDagNIYyaHR0cDovL2NybC5jb21vZG9jYS5jb20v\r\n"
// "QUFBQ2VydGlmaWNhdGVTZXJ2aWNlcy5jcmwwNqA0oDKGMGh0dHA6Ly9jcmwuY29t\r\n"
// "b2RvLm5ldC9BQUFDZXJ0aWZpY2F0ZVNlcnZpY2VzLmNybDANBgkqhkiG9w0BAQUF\r\n"
// "AAOCAQEACFb8AvCb6P+k+tZ7xkSAzk/ExfYAWMymtrwUSWgEdujm7l3sAg9g1o1Q\r\n"
// "GE8mTgHj5rCl7r+8dFRBv/38ErjHT1r0iWAFf2C3BUrz9vHCv8S5dIa2LX1rzNLz\r\n"
// "Rt0vxuBqw8M0Ayx9lt1awg6nCpnBBYurDC/zXDrPbDdVCYfeU0BsWO/8tqtlbgT2\r\n"
// "G9w84FoVxp7Z8VlIMCFlA2zs6SFz7JsDoeA3raAVGI/6ugLOpyypEBMs1OUIJqsi\r\n"
// "l2D4kF501KKaU73yqWjgom7C12yxow+ev+to51byrvLjKzg6CYG1a4XXvi3tPxq3\r\n"
// "smPi9WIsgtRqAEFQ8TmDn5XpNpaYbg==\r\n"
// "-----END CERTIFICATE-----\r\n";

// 串口初始化（含中断配置）
void UART1_Init(uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

    // 配置TX引脚（PA9）为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置RX引脚（PA10）为上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART参数
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    // 配置接收中断
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启接收中断
    USART_Cmd(USART1, ENABLE);                     // 使能串口
}

// 中断服务函数（接收数据）
__attribute__((interrupt("WCH-Interrupt-fast")))
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        uint8_t data = USART_ReceiveData(USART1); // 读取接收数据
        if (rx_cnt < RX_BUF_SIZE - 1) {          // 防止缓冲区溢出
            rx_buf[rx_cnt++] = data;
            if (data == '\n' || data == '\r') {   // 以换行符为接收结束标志
                rx_flag = 1;
                rx_buf[rx_cnt] = '\0';           // 添加字符串结束符
            }
        } else {
            rx_cnt = 0; // 溢出时清空缓冲区
        }
        USART_ClearITPendingBit(USART1, USART_IT_RXNE); // 清除中断标志
    }
}

// 自定义I/O适配层
static int wchnet_send(void *ctx, const unsigned char *buf, size_t len) {
    uint8_t socket_id = *(uint8_t*)ctx;
    uint32_t send = 0;
    uint32_t sent = 0;
    int times  = 0;
    uint8_t ret = 0;
    while (send < len) {
        sent = len - send;
        ret = WCHNET_SocketSend(socket_id, (uint8_t*)buf + send, &sent);
        send += sent;
        times++;
        if (times == 50 && send < len) {
            printf("Send insufficient data");
            break;
        }
        if(ret == WCHNET_ERR_TIMEOUT) return MBEDTLS_ERR_SSL_WANT_READ;
    }
    return (ret == WCHNET_ERR_SUCCESS) ? (int)send : MBEDTLS_ERR_SSL_INTERNAL_ERROR;
}

static int wchnet_recv(void *ctx, unsigned char *buf, size_t len) {
    uint8_t socket_id = *(uint8_t*)ctx;
    uint32_t recv = len;
    uint32_t recvd = 0;
    uint8_t ret = 0;
    int times  = 0;
    while (recvd < len) {
        recv = len - recvd;
        ret = WCHNET_SocketRecv(socket_id, (uint8_t*)buf + recvd, &recv);
        recvd += recv;
        times++;
        if (times == 50 && recvd < len) {
            break;
        }
        if(ret == WCHNET_ERR_TIMEOUT) return MBEDTLS_ERR_SSL_WANT_READ;
    }
    return (ret == WCHNET_ERR_SUCCESS) ? (int)recvd : MBEDTLS_ERR_SSL_INTERNAL_ERROR;
}

int https_post(char* userdata) {
    int ret;
    // 创建JSON请求体
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "model", MODEL_NAME);
    cJSON_AddBoolToObject(root, "stream", 0);

    cJSON *messages = cJSON_AddArrayToObject(root, "messages");
    cJSON *message  = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "role", "user");
    cJSON_AddStringToObject(message, "content",  (const char*)userdata);
    cJSON_AddItemToArray(messages, message);

    char *json_str     = cJSON_PrintUnformatted(root);
    int content_length = strlen(json_str);

    // 构建HTTP请求
    char request[1024];
    snprintf(request, sizeof(request),
             "POST /v1/chat/completions HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Authorization: Bearer %s\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Connection: keep-alive\r\n\r\n"
             "%s",
             SERVER_NAME, API_KEY, content_length, json_str);

    // 发送请求
    ret = mbedtls_ssl_write(&ssl, (const unsigned char *)request, strlen(request));
    if (ret < 0) {
        printf("mbedtls_ssl_write failed: -0x%x\n", -ret);
    }
    return ret;
}


/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (u16) iError);
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      WCHNET_CreateTcpSocket
 *
 * @brief   Create TCP Socket
 *
 * @return  none
 */
void WCHNET_CreateTcpSocket(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    memcpy((void *) TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    printf("WCHNET_CreateTcpSocket : SocketId %d\r\n", SocketId);
    mStopIfError(i);
    i = WCHNET_SocketConnect(SocketId);                        //make a TCP connection
    mStopIfError(i);
}

/*********************************************************************
 * @fn      WCHNET_DataLoopback
 *
 * @brief   Data loopback function.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void WCHNET_DataLoopback(u8 id)
{
#if 1
    u8 i;
    u32 len;
    u32 endAddr = SocketInf[id].RecvStartPoint + SocketInf[id].RecvBufLen;       //Receive buffer end address

    if ((SocketInf[id].RecvReadPoint + SocketInf[id].RecvRemLen) > endAddr) {    //Calculate the length of the received data
        len = endAddr - SocketInf[id].RecvReadPoint;
    }
    else {
        len = SocketInf[id].RecvRemLen;
    }
    i = WCHNET_SocketSend(id, (u8 *) SocketInf[id].RecvReadPoint, &len);         //send data
    if (i == WCHNET_ERR_SUCCESS) {
        WCHNET_SocketRecv(id, NULL, &len);                                       //Clear sent data
    }
#else
    u32 len, totallen;
    u8 *p = MyBuf, TransCnt = 255;

    len = WCHNET_SocketRecvLen(id, NULL);                                //query length
    printf("Receive Len = %d\r\n", len);
    totallen = len;
    WCHNET_SocketRecv(id, MyBuf, &len);                                  //Read the data of the receive buffer into MyBuf
    while(1){
        len = totallen;
        WCHNET_SocketSend(id, p, &len);                                  //Send the data
        totallen -= len;                                                 //Subtract the sent length from the total length
        p += len;                                                        //offset buffer pointer
        if( !--TransCnt )  break;                                        //Timeout exit
        if(totallen) continue;                                           //If the data is not sent, continue to send
        break;                                                           //After sending, exit
    }
#endif
}

void Respond_Parse(char* ptr)
{
    // 解析
    cJSON *root = cJSON_Parse(ptr);
    if (root == NULL) {
        return;
    }
    // 获取choices数组
    cJSON *choices = cJSON_GetObjectItem(root, "choices");
    if (choices == NULL || !cJSON_IsArray(choices)) {
        printf("未找到choices数组\n");
        cJSON_Delete(root);
        return;
    }
    
    // 获取choices数组的第一个元素
    cJSON *first_choice = cJSON_GetArrayItem(choices, 0);
    if (first_choice == NULL) {
        printf("choices数组为空\n");
        cJSON_Delete(root);
        return;
    }

    // 获取message对象
    cJSON *message = cJSON_GetObjectItem(first_choice, "message");
    if (message == NULL) {
        printf("未找到message字段\n");
        cJSON_Delete(root);
        return;
    }

    // 获取content字段的值
    cJSON *content = cJSON_GetObjectItem(message, "content");
    if (content == NULL || !cJSON_IsString(content)) {
        printf("未找到content字段\n");
        cJSON_Delete(root);
        return;
    }
    // 输出content内容
    printf("DeepSeek: %s\r\n", content->valuestring);
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    int i;
    if (intstat & SINT_STAT_RECV)                               //receive data
    {

         // 第一步：读取并解析响应头
        int ret;
        char buf[4096];
        int total_read     = 0;
        int content_length = -1;
        int is_chunked     = 0;
        size_t headers_end = 0;
        while (1) {
            ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf + total_read, sizeof(buf) - total_read - 1);
            if (ret <= 0) {
                if (ret == MBEDTLS_ERR_SSL_WANT_READ) continue; // 非阻塞模式下可能需要重试
                break;
            }
            total_read += ret;
            buf[total_read] = '\0';

            // 检查是否已读取完头部（找到 \r\n\r\n）
            headers_end = strstr(buf, "\r\n\r\n") - buf;
            if (headers_end != (size_t)-1) {
                headers_end += 4; // 跳过空行
                break;
            }
        }

        // 解析 Content-Length 或 Transfer-Encoding
        char *content_len_ptr = strstr(buf, "Content-Length: ");
        if (content_len_ptr) {
            content_length = atoi(content_len_ptr + 16);
        } else if (strstr(buf, "Transfer-Encoding: chunked")) {
            is_chunked = 1;
        }

        // 第二步：根据头部信息读取响应体
        if (content_length > 0) {
            // 按固定长度读取剩余数据
            int remaining = content_length - (total_read - headers_end);
            while (remaining > 0) {
                ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf + total_read, sizeof(buf) - total_read - 1);
                if (ret <= 0) break;
                total_read += ret;
                remaining -= ret;
            }
        } else if (is_chunked) {
            // 分块编码处理逻辑（需更复杂实现）
            // 示例简化：逐块读取直到遇到 0\r\n
            // 实际需解析块长度并跳过尾部 \r\n
            while (1) {
                ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf + total_read, sizeof(buf) - total_read - 1);
                if (ret <= 0) break;
                total_read += ret;
                if (strstr(buf, "0\r\n\r\n")) break;
            }
        }
        buf[total_read] = '\0';
        Respond_Parse(buf + headers_end);
    }
    if (intstat & SINT_STAT_CONNECT)                            //connect successfully
    {
#if KEEPALIVE_ENABLE
        WCHNET_SocketSetKeepLive(socketid, ENABLE);
#endif
        WCHNET_ModifyRecvBuf(socketid, (u32) SocketRecvBuf[socketid], RECE_BUF_LEN);
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            if (socket[i] == 0xff) {                            //save connected socket id
                socket[i] = socketid;
                break;
            }
        }
        printf("TCP Connect Success\r\n");
        printf("socket id: %d\r\n", socket[i]);
        printf("Input Your Question: \r\n");
    }
    if (intstat & SINT_STAT_DISCONNECT)                         //disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {           //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                            //timeout disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {           //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        WCHNET_CreateTcpSocket();
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt(void)
{
    u8 intstat;
    u16 i;
    u8 socketint;

    intstat = WCHNET_GetGlobalInt();                            //get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)                            //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)                          //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if (intstat & GINT_STAT_PHY_CHANGE)                         //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if (intstat & GINT_STAT_SOCKET) {                           //socket related interrupt
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                WCHNET_HandleSockInt(i, socketint);
        }
    }
}

static void SslDebug(void *ctx, int level, const char *file, int line, const char *str)
{
    printf("ssllog=== %s:%04d: %s.", file, line, str);
}

void ssl_int(void)
{
    int ret;
    mbedtls_ssl_close_notify(&ssl);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    //mbedtls_x509_crt_init(&cacert);
    //mbedtls_debug_set_threshold(1);//
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    printf("\n  . Seeding the random number generator...\n");
    //DRBG---->Deterministic Random Bit Generators 伪随机数产生器
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
        (const unsigned char*)pers,
        strlen(pers))) != 0)
    {
        printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    }
    // 2. 加载内置CA证书
    // if ((ret = mbedtls_x509_crt_parse(&cacert, (const uint8_t*)ca_cert, sizeof(ca_cert))) != 0) {
    //     printf("LINE : %d\n", __LINE__);
    // }
    // if (ret < 0)
    // {
    //     printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", (unsigned int)-ret);
    //     return;
    // }

    //MBEDTLS_SSL_IS_CLIENT 表示配置为客户端
    //MBEDTLS_SSL_TRANSPORT_STREAM 表示传输方式为TLS
    //设置版本， MBEDTLS_SSL_PRESET_DEFAULT 表示 TLS1.0
    if ((ret = mbedtls_ssl_config_defaults(&conf,
        MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\r\n", ret);
    }

    /*设置数字证书检查模式
    *  MBEDTLS_SSL_VERIFY_NONE:      peer certificate is not checked
    *                        (default on server)
    *                        (insecure on client)
    *
    *  MBEDTLS_SSL_VERIFY_OPTIONAL:  peer certificate is checked, however the
    *                        handshake continues even if verification failed;
    *                        mbedtls_ssl_get_verify_result() can be called after the
    *                        handshake is complete.
    *
    *  MBEDTLS_SSL_VERIFY_REQUIRED:  peer *must* present a valid certificate,
    *                        handshake is aborted if verification failed.
    *                        (default on client)
    */
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    //mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    // 配置随机数生成器的回调函数
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    // 配置调试回调函数
    // mbedtls_ssl_conf_dbg(&conf, SslDebug, NULL);
    // 根据conf设置ssl结构
    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
    {
        printf(" failed\n  ! mbedtls_ssl_setup returned 0x%x\r\n", -ret);
    }
    // 设置host name 用到动态内存分配(单向认证可以屏蔽)
	if( ( ret = mbedtls_ssl_set_hostname( &ssl, "api.siliconflow.cn" ) ) != 0 )
	{
		printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\r\n", ret );
	}
    // 设置发送和接收接口
    mbedtls_ssl_set_bio(&ssl, &SocketId, wchnet_send, wchnet_recv, NULL);
}

u8 WCHNET_DHCPCallBack(u8 status, void *arg)
{
    u8 *p;
    u8 tmp[4] = {0, 0, 0, 0};

    if(!status)
    {
        p = arg;
        printf("DHCP Success\r\n");
        /*If the obtained IP is the same as the last IP, exit this function.*/
        if(!memcmp(IPAddr, p ,sizeof(IPAddr)))
            return READY;
        /*Determine whether it is the first successful IP acquisition*/
        if(memcmp(IPAddr, tmp ,sizeof(IPAddr))){
            /*The obtained IP is different from the last value,
             * then disconnect the last connection.*/
            WCHNET_SocketClose(SocketId, TCP_CLOSE_NORMAL);
        }
        memcpy(IPAddr, p, 4);
        memcpy(GWIPAddr, &p[4], 4);
        memcpy(IPMask, &p[8], 4);
        printf("IPAddr: %d.%d.%d.%d \r\n", (u16)IPAddr[0], (u16)IPAddr[1],
               (u16)IPAddr[2], (u16)IPAddr[3]);
        printf("GWIPAddr: %d.%d.%d.%d \r\n", (u16)GWIPAddr[0], (u16)GWIPAddr[1],
               (u16)GWIPAddr[2], (u16)GWIPAddr[3]);
        printf("IPMask: %d.%d.%d.%d \r\n", (u16)IPMask[0], (u16)IPMask[1],
               (u16)IPMask[2], (u16)IPMask[3]);
        printf("DNS1: %d.%d.%d.%d \r\n", p[12], p[13], p[14], p[15]);
        printf("DNS2: %d.%d.%d.%d \r\n", p[16], p[17], p[18], p[19]);
        WCHNET_CreateTcpSocket();                                                   //Create a TCP connection
        return READY;
    }
    else
    {
        printf("DHCP Fail %02x \r\n", status);
        /*Determine whether it is the first successful IP acquisition*/
        if(memcmp(IPAddr, tmp ,sizeof(IPAddr))){
            /*The obtained IP is different from the last value*/
            WCHNET_SocketClose(SocketId, TCP_CLOSE_NORMAL);
        }
        return NoREADY;
    }
}

static void ssl_debug(void *ctx, int level, const char *file, int line, const char *str) {
    printf("[MBEDTLS] %s:%04d | %s", file, line, str);
}


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    u8 i;
    mbedtls_memory_buffer_alloc_init(static_mem_pool, sizeof(static_mem_pool));
    // mbedtls_debug_set_threshold(0); // 2 是中等详细级别
    // mbedtls_platform_set_printf(printf);
    SystemCoreClockUpdate();
    Delay_Init();
    // USART_Printf_Init(115200);                                  //USART initialize
    UART1_Init(115200); // 初始化串口1，波特率115200
    printf("TCPClient Test\r\n");
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("net version:%x\n", WCHNET_GetVer());
    if (WCHNET_LIB_VER != WCHNET_GetVer()) {
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                 //get the chip MAC address
    printf("mac addr:");
    for(i = 0; i < 6; i++)
        printf("%x ", MACAddr[i]);
    printf("\n");
    TIM2_Init();
    // WCHNET_DHCPSetHostname("WCHNET"); 
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);         //Ethernet library initialize
    mStopIfError(i);  
    if (i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
    i = WCHNET_DHCPStart(WCHNET_DHCPCallBack);                                //Start DHCP
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_DHCPStart Success\r\n");
    ssl_int(); 
#if KEEPALIVE_ENABLE                                            //Configure keep alive parameters
    {
        struct _KEEP_CFG cfg;

        cfg.KLIdle = 20000;
        cfg.KLIntvl = 15000;
        cfg.KLCount = 9;
        WCHNET_ConfigKeepLive(&cfg);
    }
#endif
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        WCHNET_CreateTcpSocket();                               //Create TCP Socket
    
    while(1)
    {
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        WCHNET_MainTask();
        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        if(WCHNET_QueryGlobalInt())
        {
            WCHNET_HandleGlobalInt();
        }

        if (rx_flag) { // 检测到接收完成标志
            printf("UART Received: %s\r\n", (char *)rx_buf); // 回显接收的数据
            bSendSocket = TRUE;
            rx_flag = 0;
            rx_cnt = 0;
            https_post((char *)rx_buf);
        }
    }
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    // mbedtls_x509_crt_free(&cacert);
}