# coding:utf-8
from twilio.rest import Client

# put your own credentials here
account_sid = ""
auth_token = ""

client = Client(account_sid, auth_token)

def send_sms1():

    client = Client(account_sid, auth_token)

    message = client.messages.create(
        to="+8615673342006",
        from_="+15742164335",
        body="检测到物体移动")

def send_sms2():

    client = Client(account_sid, auth_token)

    message = client.messages.create(
        to="+8615673342006",
        from_="+15742164335",
        body="系统检测到温度过高")

def send_sms3():

    client = Client(account_sid, auth_token)

    message = client.messages.create(
        to="+8615673342006",
        from_="+15742164335",
        body="空气重度污染")
    
'''
client.messages.create(
    to="+8615673342006",
    from_="+15742164335",
    body="中文显示正常，检测到物体移动，系统检测到温度过高，空气重度污染")
'''
