index = 0x1200	subindex = 3    -----进入bootloader模式消息  m->data[0] = 0x23 m->data[0] = 0x40

index = 0x1201	subindex = 3	-----1号lvdt最小位置标定消息

index = 0x1202	subindex = 3	-----2号lvdt最小位置标定消息

index = 0x1203	subindex = 3	-----1号lvdt最大位置标定消息

index = 0x1204	subindex = 3	-----2号lvdt最大位置标定消息

index = 0x1205	subindex = 3	-----读取1号lvdt当前位置消息

index = 0x1206	subindex = 3	-----读取2号lvdt当前位置消息

index =0x1207	subindex = 3	----设置1号lvdt零位置消息

index =0x1208	subindex = 3	----设置2号lvdt零位置消息

index = 0x1209	subindex = 3	------设置第一联id消息

index = 0x120a	subindex = 3	------保存设置id消息

index = 0x120b	subindex = 3 -------启动分段下载请求,数据内容为要下载字节数

index = 1	subindex = 3--------主动上报lvdt位置

index = 1	subindex = 3--------主动上报节点ID

index = 1	subindex = 3---------下载进度百分比

index = 1	subindex = 3---------正在擦除flash消息

index = m->data[1] << 8 | m->data[2]

subindex = m->data[3]


master_nodeid = 0x58A------------上位机
slave_nodeid = 0x60A--------------板卡
