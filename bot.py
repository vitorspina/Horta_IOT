#Imports 
import json
from telebot import TeleBot
import os
from dotenv import load_dotenv
from get_temp_data import gettempesp
from get_db_info import get_db_info
import get_temp_data
import time
load_dotenv()

#Classes para ajudar o codigo 
gettemp = gettempesp("GetTEMPTELEGRAMBOT")
gettemp.connect(os.environ['broker_adress'], 1883)
gettemp.loop_start()
c = get_db_info(os.environ['HOST_SQL'],os.environ['USER_SQL'],os.environ['PASSWORD_SQL'])



# functions uteis

def hortinha(message):
    if message.text.lower() == "dados":
        return True
    return False


#Bot
bot = TeleBot(os.environ["API_KEY"])

#Handlers

@bot.message_handler(commands = ["start"])
def start (message):
    print(message.chat.id)
    resultado = c.get_info(message.chat.id)
    if resultado!= None:
        bot.send_message(resultado[1],f"Olá {resultado[0]} Bem-vindo novamente, bons trabalhos")
    else:
        bot.send_message(message.chat.id,"Bem vindo estranho ao bot em desenvolvimento")

@bot.message_handler(func= hortinha)
def get_temperatura(message):
    
    
    gettemp.publish("esp/situacao","a") 
    time.sleep(3)

    resultado = c.get_info(message.chat.id)
    print(get_temp_data.dados)
    valores = get_temp_data.dados
    if resultado!= None:
        bot.send_message(resultado[1],f"Olá {resultado[0]}, agora são {valores[3]},\n sua hortinha está assim:\n {valores[0]}°C,\n {valores[1]}%,\n Luminosidade: {valores[2]},\n Umidade do solo: {valores[4]}")
    else:
        bot.send_message(message.chat.id,"id não valido")

@bot.message_handler(commands=["cera"])
def cera(message):
    bot.send_photo(message.chat.id , "https://upload.wikimedia.org/wikipedia/commons/8/8e/Michael_Cera_2012_%28Cropped%29.jpg")





# Bot long polling process
bot.polling()