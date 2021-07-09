import mysql.connector
class get_db_info():
    def __init__(self,host,user,password):
        self.host = host
        self.user = user
        self.password = password
    def get_info(self,iddochat):
        mydb = mysql.connector.connect(
          host=self.host,
          user=self.user,
          password=self.password
        )
        
        mycursor = mydb.cursor()

        mycursor.execute("use telegrambot")
        mycursor.execute(f"SELECT * FROM usuarios where chat_id = {iddochat}")

        myresult = mycursor.fetchone()
        
        mycursor.close()
        return myresult


