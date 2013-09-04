/*
    AstroZaur - Universal astrologic processor
    Copyright (C) 2006 Belousov Oleg <belousov.oleg@gmail.com>
    http://www.strijar.ru/astrozaur

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "dbus.h"
#include "lua.h"
#include "program.h"
#include "map_natal.h"

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fltk/run.h>
#include <fltk/Threads.h>

CDBus	dbus;

static fltk::Thread	iThread;

static void* DBusLoop(void *) {
    while (1) {
	dbus.Check();
    }
}

static DBusHandlerResult disconnect_filter(DBusConnection *conn,
						DBusMessage *msg, void *data)
{
    const char *iface,*method;
//struct disconnect_data *dc_data = data;

    if (dbus_message_get_type (msg) != DBUS_MESSAGE_TYPE_SIGNAL)
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    iface = dbus_message_get_interface(msg);
    method = dbus_message_get_member(msg);

    if ((strcmp(iface, DBUS_INTERFACE_LOCAL) == 0) &&
	(strcmp(method, "Disconnected") == 0))
    {
	fprintf(stderr, "Got disconnected from the system message bus");
	    dbus_connection_unref(conn);
	    dbus.Connect();
//		dc_data->disconnect_cb(dc_data->user_data);
    }
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

CDBus::CDBus() {
    iConn = NULL;
    iMsg = NULL;
    Connect();
}

void CDBus::Connect() {
    dbus_error_init(&iErr);
   
    iConn = dbus_bus_get(DBUS_BUS_SESSION, &iErr);
    
    if (dbus_error_is_set(&iErr)) { 
        fprintf(stderr, "DBus: Connection Error (%s)\n", iErr.message); 
        dbus_error_free(&iErr);
    }
    
    if (iConn == NULL) {
        fprintf(stderr, "DBus: Connection Null\n"); 
	return;
    }
	
    dbus_connection_set_exit_on_disconnect(iConn, FALSE);

    if (!dbus_connection_add_filter(iConn, disconnect_filter, NULL, free)) {
	fprintf(stderr, "DBus: Can't add disconnect filter");
        dbus_connection_unref(iConn);
        return;
    }
}

void CDBus::AddMatch(const char* match) {
    if (iConn) {
	dbus_bus_add_match(iConn, match, &iErr);
	dbus_connection_flush(iConn);
    
	if (dbus_error_is_set(&iErr)) { 
    	    fprintf(stderr, "DBus: Match Error (%s)\n", iErr.message);
	}
    }
}

void CDBus::RequestName(const char* name) {
    if (iConn) {
	int ret = dbus_bus_request_name(iConn, name, DBUS_NAME_FLAG_REPLACE_EXISTING, &iErr);
    
	if (dbus_error_is_set(&iErr)) { 
    	    fprintf(stderr, "DBus: Name Error (%s)\n", iErr.message); 
    	    dbus_error_free(&iErr);
	}
   
	if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) { 
    	    fprintf(stderr, "DBus: Not Primary Owner (%d)\n", ret);
	}
    }
}


void CDBus::Start() {
    if (iConn) {
	fltk::lock();
	fltk::create_thread(iThread, DBusLoop, NULL);
	fltk::unlock();
    }
}

CDBus::~CDBus() {
}

void CDBus::Check() {
    DBusMessage*	msg;
    DBusMessageIter 	args;

    dbus_connection_read_write(iConn, 10);
    msg = dbus_connection_pop_message(iConn);

    if (msg == NULL) return;
    
    switch (dbus_message_get_type(msg)) {
	case DBUS_MESSAGE_TYPE_SIGNAL:
	    char** path;
	
	    dbus_message_get_path_decomposed(msg, &path);
	    
	    if (path[0]) {
		if (strcmp(path[0], "current") == 0) {
		    if (path[1] && strcmp(path[1], "position") == 0) {
			double	lon, lat;
			char*	id;
		    
			dbus_message_get_args(msg, &iErr, 
			    DBUS_TYPE_DOUBLE, &lon, 
			    DBUS_TYPE_DOUBLE, &lat,
			    DBUS_TYPE_STRING, &id);
			    
			if (dbus_error_is_set(&iErr)) {
    			    fprintf(stderr, "DBus: Name Error (%s)\n", iErr.message); 
    			    dbus_error_free(&iErr);
			} else {
			    CMapNatal* map = (CMapNatal*) program.Map(id);
			    
			    if (map) {
			        fltk::lock();

				map->Place()->Lon(lon);
				map->Place()->Lat(lat);
				
				program.Event(map->Place(), eventChangePlaceLon);
				program.Event(map->Place(), eventChangePlaceLat);
				
				program.CalcMaps();
				program.Draw();
				
				fltk::unlock();
				fltk::awake(0);
			    }
			}
		    }
		} else if (strcmp(path[0], "lua") == 0) {
    		    if (dbus_message_iter_init(msg, &args) && dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_STRING) {
			char* str;
		    
        		dbus_message_iter_get_basic(&args, &str);
			lua.Exec(str);
		    } else {
			printf("DBus: Lua error arg\n");
		    }
		}
	    }
	    dbus_free_string_array(path);
	    break;
    }
    dbus_message_unref(msg);
}

void CDBus::NewSignal(const char* obj, const char* interface, const char* name, const char* dest) {
    if (iConn) {
	iMsg = dbus_message_new_signal(obj, interface, name);
    
	if (iMsg == NULL) { 
    	    fprintf(stderr, "DBus: Error New Message\n");
	    return;
	}
	
	if (dest) {
	    printf("Dest: [%s]\n", dest);
	    if (!dbus_message_set_destination(iMsg, name))
		fprintf(stderr, "DBus: Error set dest\n");
	}
    }
}

void CDBus::AppendArg(double data) {
    if (iMsg) {
	dbus_message_append_args(iMsg, DBUS_TYPE_DOUBLE, &data, DBUS_TYPE_INVALID);
    }
}

void CDBus::AppendArg(char* data) {
    if (iMsg) {
	dbus_message_append_args(iMsg, DBUS_TYPE_STRING, &data, DBUS_TYPE_INVALID);
    }
}

void CDBus::AppendArg(unsigned int data) {
    if (iMsg) {
	dbus_message_append_args(iMsg, DBUS_TYPE_UINT32, &data, DBUS_TYPE_INVALID);
    }
}

void CDBus::Send() {
    if (iMsg) {
        dbus_uint32_t serial = 0;

	if (!dbus_connection_send(iConn, iMsg, &serial)) {
	    fprintf(stderr, "Out Of Memory!\n"); 
    	    return;
	}
   
//	dbus_connection_flush(iConn);
	dbus_message_unref(iMsg);
	iMsg = NULL;
    }
}
