import paho.mqtt.client as mqtt
import paho.mqtt.client

dados = ''


class gettempesp(mqtt.Client):
    def __init__(self, client_id='', clean_session=None, userdata=None, protocol=paho.mqtt.client.MQTTv311, transport='tcp'):
        super().__init__(client_id=client_id, clean_session=clean_session, userdata=userdata, protocol=protocol, transport=transport)
    def on_connect(self, mqttc, obj, flags, rc):
        print("rc: "+str(rc))
        self.subscribe("retornatemp")
    def on_message(self, mqttc, obj, msg):
        global dados
        valor = str(msg.payload).replace("b","").replace("'","")
        dados = valor.split("/n")#[:-67]
        
            

