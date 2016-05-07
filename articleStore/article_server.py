import psycopg2
import pprint

from flask import Flask, jsonify
from flask.ext.compress import Compress
app = Flask(__name__)
Compress(app)

@app.route("/")
def hello():
    c = conn.cursor()
    start_timestamp = "2016-01-15 00:00:00"
    end_timestamp = "2016-01-31 23:59:59"

    c.execute("select id, title, short_desc, content_body from articles where publish_date >='" + start_timestamp+"' and publish_date <='"+ end_timestamp + "'");
    rs = c.fetchall()
    return jsonify(articles=rs)

if __name__ == "__main__":
    conn = psycopg2.connect("dbname='woi_production'")
    app.run(debug=True)
