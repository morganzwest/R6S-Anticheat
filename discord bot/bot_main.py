import discord, firebase_api, requests
from discord.ext import commands
import random

TOKEN = 'NzY1NjE5MDIzMzc0MzE5NjE5.X4XcRw.IHNOC2_PxWsXVNewgR6F0bqYtFI'
PREFIX = "!"

class MyClient(discord.Client):
    async def on_ready(self):

        self.db = firebase_api.Db(
            "https://project-b-5b43c.firebaseio.com/",
            "key.json"
        )

        print('Logged in as')
        print(self.user.name)
        print(self.user.id)
        print('------')

    @staticmethod
    def isCommand(message, command, params = False):
        if params:
            if message.content.startswith(PREFIX) and command in message.content[1:]:
                return message[1:]
        else:
            if message.content.startswith(PREFIX) and message.content.endswith(command):
                print("Command Call: ", message.author.name)
                print("Command: ", command)
                print("------")
                return True

    async def on_message(self, message):
        # we do not want the bot to reply to itself
        if message.author.id == self.user.id:
            return

        if message.content.startswith('!hello'):
            await message.channel.send('Hello {0.author.mention}'.format(message))

        if self.isCommand(message, "api"):
            await message.channel.send("API WOO IT WORKS FUCK YEAH")

        if self.isCommand(message, "admin raw"):
            local_data = requests.get('https://project-b-5b43c.firebaseio.com/.json').text
            await message.channel.send(local_data)

        if self.isCommand(message, "admin indexes"):
            data = self.db.load("/test/").get()
            await message.channel.send(len(data))

        if self.isCommand(message, "admin index search", params = True):
            data = self.db.load("/test/").get()
            await message.channel.send(len(data))


client = MyClient()
client.run(TOKEN)