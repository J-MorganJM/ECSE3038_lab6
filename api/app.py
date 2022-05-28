import os
from datetime import datetime
from flask import Flask, jsonify, request
from flask_cors import CORS
from flask_pymongo import PyMongo
from marshmallow import Schema, ValidationError, fields
from bson.json_util import dumps
from json import loads
from dotenv import load_dotenv

load_dotenv()

app = Flask(__name__)
CORS(app)
app.config["MONGO_URI"] = os.getenv("MONGO_CONNECTION_STRING")
mongo = PyMongo(app)

# Tank Schema
class Level(Schema):
	tank_id = fields.String(required=True)
	water_level=fields.Integer(required=True)

# Create/POST Request
@app.route("/tank",methods=["POST"])
def stat():
    ledstatus = False
    request_dict = request.json 
    try: 
        new_tank = Level().load(request_dict)
    except ValidationError as err:
        return(err.messages, 400)
    
    tank_document = mongo.db.levels.insert_one(new_tank)
    tank_id = tank_document.inserted_id
    tank = mongo.db.levels.find_one({"_id": tank_id})
    tank_json = loads(dumps(tank))

    #Get current time - Snippet from : https://www.programiz.com/python-programming/datetime/current-time
    now = datetime.now()
    current_time = str(now.strftime("%H:%M:%S"))

    if tank_json["water_level"] in range(27,35):
        ledstatus=True
    message_dict={
	"led": ledstatus,
	"msg": "data saved in database successfully",
	"date": current_time
    }
    return jsonify(message_dict)

if __name__ == '__main__':
    app.run(
    debug=True, 
    host="0.0.0.0", 
    port="3000"
    )
