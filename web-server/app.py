#加载文件，mqtt客户端，服务端，
# coding:utf-8
import paho.mqtt.client as mqtt
from flask import Flask, render_template, request, session, url_for
from flask_socketio import SocketIO, emit
from flask import *
from twilio.rest import Client
import time


#配置Flask，和soketio
app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)



# 服务端收到客服端连接请求的响应The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("hut/temperature")
    client.subscribe("hut/humidity")
    client.subscribe("hut/mq135")
    client.subscribe("hut/pir")

# 订阅连接上ESP8266客户端后的，接收数据处理，通过socketio实时发送给前端The callback for when a PUBLISH message is received from the ESP8266.
def on_message(client, userdata, message):

    print("Received message '" + str(message.payload) + "' on topic '"
        + message.topic + "' with QoS " + str(message.qos))
    if message.topic == "hut/temperature":
        print("temperature update")
	socketio.emit('temperature', {'data': message.payload})
    if message.topic == "hut/humidity":
        print("humidity update")
        socketio.emit('humidity', {'data': message.payload})
    if message.topic == "hut/mq135":
        print("airquality update")
        socketio.emit('airquality', {'data': message.payload})
    if message.topic == "hut/pir":
        print("pir update")
        socketio.emit('bellalert', {'data': message.payload})



#调用MQTT接口函数		
mqttc = mqtt.Client()
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.connect("localhost",1883,60)
mqttc.loop_start()




#注册页路由设置
@app.route("/login",methods=['POST','GET'])
def login():
    error=None
    if request.method == 'POST':
        session['name'] = request.form['username']
        session['password'] = request.form['password']
   	if request.form['username'] != 'admin' or request.form['password'] != 'admin123':
            error= "sorry"
        else:
            return redirect(url_for('main'))
    return render_template('login.html',error=error)

#主页面路由设置
@app.route("/homepage",methods=['POST','GET'])
def main():
    if session.get('name')=='admin' and session.get('password')=='admin123':
        # Pass the template data into the template main.html and return it to the user
        return render_template('index.html', async_mode=socketio.async_mode)
    else:
        return redirect(url_for('login'))


#从网页端接收的数据，并进行实时处理，发布给ESP8266
@socketio.on('my event')
def handle_my_custom_event(json):
    print('received json data here: ' + str(json))
    
@socketio.on('rgb')
def handle_rgb_event(msg):
    print('received rgb data here: ' + msg)
    mqttc.publish('hut/rgb', msg)
    
@socketio.on('fan')
def handle_fan_event(msg):
    print('received fan data here: ' + str(msg))
    if msg == 1:
        mqttc.publish('hut/fan', "1")
        mqttc.publish('hut/fan', "1")
        print("1  OKay!")
    else:
        mqttc.publish('hut/fan', "0")
        mqttc.publish('hut/fan', "0")

@socketio.on('curtain')
def handle_curtain_event(msg):
    print('received curtain data here: ' + str(msg))
    if msg == 0:
        mqttc.publish('hut/step', "0")
    if msg == 1:
        mqttc.publish('hut/step', "1")


#运行后台，映射IP地址，
if __name__ == "__main__":
    socketio.run(app, host='192.168.191.20', port=8181)
