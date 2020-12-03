import argparse, firebase_api
# import datetime

parser = argparse.ArgumentParser(description = 'Export API')
parser.add_argument('export',
                    metavar = 'STRING',
                    type = list,
                    nargs = '+',
                    help = "String for output"
                    )

args = parser.parse_args()

orstring = ""
for letter in args.export[0]:
    orstring += letter

# replace $ with ' '
string = orstring.replace("$", " ")
# split into array of outputs
string = string.split("@")

db = firebase_api.Db(
    "https://project-b-5b43c.firebaseio.com/",
    "key.json"
)

#2012-05-06.21:47:59
#print("API:DEBUG:", orstring)
time = string[-1].replace(".", "").replace("-", "").replace(":", "") #+ str(datetime.datetime.utcnow().microsecond)
buffer = str(string[2].split(":")[1])

db.update(
    "/test/",
    {
        time + ':' + buffer: string[:3]
    }
)
