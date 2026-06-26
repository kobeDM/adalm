#!/usr/bin/python3

import sys
import time
import logging
from watchdog.observers import Observer
from watchdog.events import LoggingEventHandler
import datetime

from influxdb import InfluxDBClient

class RateLoggingEventHandler( LoggingEventHandler ):
    def on_modified( self, event ):

        if event.is_directory == False:
            target_filepath = event.src_path
            with open( target_filepath, "r" ) as f:
                last_line = f.readlines()[-1]

            rate = (float)(last_line.split( )[ 1 ])
            ratetime = datetime.datetime.utcnow()
            json_data = [
                {
                    'measurement' : 'rate',
                    'fields' : {
                        'real_rate' : rate,
                    },
                    'time' : ratetime,
                    'tags' : {
                        'host' : 'na177',
                        'device' : 'na177',
                    }
                }
            ]
            result = client.write_points( json_data )    


if __name__ == "__main__":

    client = InfluxDBClient( host = "localhost",
                             port = "8086",
                             username = "root",
                             password = "root",
                             database = "KUBEAM")

    logging.basicConfig( level = logging.INFO,
                         format = '%(asctime)s - %(message)s',
                         datefmt = '%Y-%m-%d %H:%M:%S' )
    path = sys.argv[1] if len( sys.argv ) > 1 else '.'
    event_handler = RateLoggingEventHandler( )
    observer = Observer( )
    observer.schedule( event_handler, path, recursive = True )
    observer.start( )
    try:
        while observer.is_alive( ):
            observer.join( 1 )
    except KeyboardInterrupt:
        observer.stop( )
        observer.join( )
    finally:
        observer.stop( )
        observer.join( )
        
