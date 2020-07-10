
import socket
import os
import time
import numpy as np

server = socket.socket()
server.bind(('127.0.0.1',9999))
server.listen(5)

while True:
    print('等待连接...')
    con,addr = server.accept()
    while True:
        try:
            data=np.zeros((255),dtype=int)
            data=con.recv(10240) # 接收数据
            if data[0] == 5:
                if data[1] == 80:
                    con.send('findheader'.encode())
            length = data[2] + data[3]*256
            
            newtemp = np.zeros((length,),dtype=int)
            
            td = length
            while td>0:
                data = con.recv(10240)
                if data[1]!= 81:
                    break
                else:
                    pn = data[2]
                    for i in range(0,data[3]):
                        newtemp[pn*250+i] = data[i+4]
                        
                    con.send('findpackage'.encode())
                    print(data[2])
                td-=data[3]
            data=con.recv(10240)
            if data[0] == 5:
                if data[1] == 82:
                    con.send('findlast'.encode())
            print(data[6],td)        
        except ConnectionResetError as e:
            print('客户端：%s 已经断开！'%addr[0])
            break
        break            
    tot = 0
    for i in range(0,length):
        tot += newtemp[i]
        print(newtemp[i])
    print(tot)    
    if tot%256 == data[6]:
        print('check ok Nice')
        
    con.close()
server.close()
