CC = g++
CFLAGS = -O2 -Wall -pipe

#DBUS = yes

FLTK_INC = $(shell ../fltk-2.0.x-r5272/fltk2-config --cflags)
FLTK_LIB = $(shell ../fltk-2.0.x-r5272/fltk2-config --libs --ldflags --use-images)

XML_INC = $(shell xml2-config --cflags)
XML_LIB = $(shell xml2-config --libs)

LUA_INC = -I/usr/local/include
LUA_LIB = -L/usr/local/lib -llua

CFLAGS += $(FLTK_INC) $(XML_INC) $(LUA_INC) -I./
LIBS += $(FLTK_LIB) $(XML_LIB) $(LUA_LIB) -L./sweph -lswe -lsqlite3 -ldl

LIBS += -g
CFLAGS += -g

#LIBS += -pg
#CFLAGS += -pg

NAME = astrozaur
OBJS = aspects.o aspect_eql_lon.o aspect_equator.o aspect_reception.o aspect_symetric.o\
	astro.o atlas.o base_window.o coord.o date.o division.o draw_area.o\
	line.o log.o\
	lua.o lua_atlas.o lua_sqlite.o lua_gui.o lua_swe.o\
	main.o\
	map.o map_edit.o\
	map_aspect.o\
	map_develop.o map_develop_edit.o\
	map_multy.o\
	map_direct.o\
	map_progres.o\
	map_eqlipse.o map_eqlipse_edit.o\
	map_harmonic.o\
	map_ingres.o\
	map_date_select.o\
	map_middle.o\
	map_natal.o map_natal_edit.o\
	map_raw.o map_raw_edit.o\
	map_revolution.o\
	map_stationar.o\
	maps.o\
	place.o point.o point_aspect.o\
	program.o\
	projection.o\
	resolve_aspect.o\
	static_holder.o\
	timezone.o\
	transform_equator.o transform_horizont.o transform_sidereal.o transform_inmundo.o\
	window.o utils.o

ifdef DBUS
    OBJS += dbus.o 
    CFLAGS += -DDBUS
    CFLAGS += -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include/
    CFLAGS += += -I/usr/local/include/dbus-1.0 -I/usr/local/lib/dbus-1.0/include/
    LIBS += -ldbus-1
endif

.cpp.o:
	$(CC) -c $(CFLAGS) $<     

.c.o:
	$(CC) -c $(CFLAGS) $<     

$(NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	/bin/rm -f *.o *~ $(NAME)

depend:
	makedepend -Y -I. *.cpp

# DO NOT DELETE

aspect_eql_lon.o: point.h coord.h astro.h utils.h draw_area.h
aspect_eql_lon.o: aspect_eql_lon.h aspects.h lua.h
aspect_equator.o: aspect_equator.h aspects.h astro.h point.h coord.h
aspect_equator.o: draw_area.h line.h lua.h map.h utils.h
aspect_reception.o: aspect_reception.h aspects.h astro.h point.h coord.h
aspect_reception.o: draw_area.h line.h lua.h map.h utils.h division.h
aspects.o: aspects.h astro.h aspect_eql_lon.h aspect_equator.h
aspects.o: aspect_reception.h aspect_symetric.h map.h point.h coord.h utils.h
aspects.o: lua.h
aspect_symetric.o: aspect_symetric.h aspects.h astro.h point.h coord.h
aspect_symetric.o: draw_area.h line.h lua.h map.h utils.h program.h maps.h
astro.o: astro.h utils.h program.h maps.h map.h point.h coord.h lua.h
atlas.o: atlas.h base_window.h astro.h map_natal.h map.h point.h coord.h
atlas.o: utils.h date.h timezone.h place.h program.h maps.h lua.h
base_window.o: base_window.h astro.h utils.h lua.h
coord.o: coord.h astro.h
date.o: utils.h lua.h date.h astro.h timezone.h program.h maps.h map.h
date.o: point.h coord.h
dbus.o: dbus.h lua.h program.h astro.h maps.h map.h point.h coord.h utils.h
dbus.o: map_natal.h date.h timezone.h place.h
division.o: division.h astro.h utils.h static_holder.h
draw_area.o: window.h maps.h astro.h map.h point.h coord.h utils.h
draw_area.o: draw_area.h base_window.h program.h projection.h lua.h
line.o: line.h astro.h lua.h utils.h draw_area.h
log.o: log.h base_window.h astro.h utils.h program.h maps.h map.h point.h
log.o: coord.h lua.h lua_gui.h
lua.o: lua.h lua_sqlite.h lua_gui.h lua_swe.h program.h astro.h maps.h map.h
lua.o: point.h coord.h utils.h division.h static_holder.h
lua_gui.o: lua.h lua_gui.h program.h astro.h maps.h map.h point.h coord.h
lua_gui.o: utils.h
lua_sqlite.o: lua.h
lua_swe.o: lua.h
main.o: astro.h program.h maps.h map.h point.h coord.h utils.h division.h
main.o: lua.h
map_aspect.o: utils.h program.h astro.h maps.h map.h point.h coord.h
map_aspect.o: map_aspect.h map_develop.h map_natal.h date.h timezone.h
map_aspect.o: place.h resolve_aspect.h lua.h
map.o: astro.h map.h point.h coord.h utils.h map_edit.h base_window.h
map.o: program.h maps.h draw_area.h transform_horizont.h transform.h
map.o: transform_sidereal.h transform_inmundo.h transform_equator.h lua.h
map.o: point_aspect.h resolve_aspect.h date.h timezone.h
map_date_select.o: map_date_select.h map_multy.h map_natal.h map.h astro.h
map_date_select.o: point.h coord.h utils.h date.h timezone.h place.h
map_date_select.o: program.h maps.h
map_develop.o: utils.h program.h astro.h maps.h map.h point.h coord.h
map_develop.o: map_develop.h map_natal.h date.h timezone.h place.h
map_develop.o: map_develop_edit.h map_natal_edit.h map_edit.h base_window.h
map_develop.o: lua.h
map_develop_edit.o: program.h astro.h maps.h map.h point.h coord.h utils.h
map_develop_edit.o: map_develop.h map_natal.h date.h timezone.h place.h
map_develop_edit.o: map_develop_edit.h map_natal_edit.h map_edit.h
map_develop_edit.o: base_window.h
map_direct.o: utils.h lua.h program.h astro.h maps.h map.h point.h coord.h
map_direct.o: map_direct.h map_develop.h map_natal.h date.h timezone.h
map_direct.o: place.h
map_edit.o: map_edit.h map.h astro.h point.h coord.h utils.h base_window.h
map_edit.o: program.h maps.h lua.h lua_gui.h
map_eqlipse.o: utils.h map_eqlipse.h map_develop.h map_natal.h map.h astro.h
map_eqlipse.o: point.h coord.h date.h timezone.h place.h map_eqlipse_edit.h
map_eqlipse.o: map_natal_edit.h map_edit.h base_window.h program.h maps.h
map_eqlipse_edit.o: program.h astro.h maps.h map.h point.h coord.h utils.h
map_eqlipse_edit.o: map_eqlipse.h map_develop.h map_natal.h date.h timezone.h
map_eqlipse_edit.o: place.h map_eqlipse_edit.h map_natal_edit.h map_edit.h
map_eqlipse_edit.o: base_window.h
map_harmonic.o: utils.h program.h astro.h maps.h map.h point.h coord.h
map_harmonic.o: map_harmonic.h map_natal.h date.h timezone.h place.h
map_ingres.o: utils.h program.h astro.h maps.h map.h point.h coord.h
map_ingres.o: map_ingres.h map_natal.h date.h timezone.h place.h lua.h
map_middle.o: map_middle.h map_multy.h map_natal.h map.h astro.h point.h
map_middle.o: coord.h utils.h date.h timezone.h place.h
map_multy.o: map_multy.h map_natal.h map.h astro.h point.h coord.h utils.h
map_multy.o: date.h timezone.h place.h program.h maps.h lua.h
map_natal.o: program.h astro.h maps.h map.h point.h coord.h utils.h
map_natal.o: map_natal.h date.h timezone.h place.h map_natal_edit.h
map_natal.o: map_edit.h base_window.h lua.h
map_natal_edit.o: map_natal.h map.h astro.h point.h coord.h utils.h date.h
map_natal_edit.o: timezone.h place.h map_natal_edit.h map_edit.h
map_natal_edit.o: base_window.h program.h maps.h atlas.h
map_progres.o: utils.h map_progres.h map_develop.h map_natal.h map.h astro.h
map_progres.o: point.h coord.h date.h timezone.h place.h program.h maps.h
map_progres.o: lua.h
map_raw.o: map_raw.h map.h astro.h point.h coord.h utils.h map_raw_edit.h
map_raw.o: map_edit.h base_window.h program.h maps.h
map_raw_edit.o: map_raw.h map.h astro.h point.h coord.h utils.h
map_raw_edit.o: map_raw_edit.h map_edit.h base_window.h program.h maps.h
map_revolution.o: utils.h program.h astro.h maps.h map.h point.h coord.h
map_revolution.o: lua.h map_revolution.h map_natal.h date.h timezone.h
map_revolution.o: place.h
maps.o: program.h astro.h maps.h map.h point.h coord.h utils.h projection.h
maps.o: window.h draw_area.h base_window.h aspects.h lua.h
map_stationar.o: utils.h program.h astro.h maps.h map.h point.h coord.h
map_stationar.o: map_stationar.h map_natal.h date.h timezone.h place.h
place.o: place.h coord.h astro.h utils.h lua.h program.h maps.h map.h point.h
point_aspect.o: point_aspect.h point.h coord.h astro.h resolve_aspect.h
point_aspect.o: date.h timezone.h map.h utils.h
point.o: map.h astro.h point.h coord.h utils.h map_natal.h date.h timezone.h
point.o: place.h draw_area.h program.h maps.h lua.h aspects.h line.h
program.o: maps.h astro.h map.h point.h coord.h utils.h program.h window.h
program.o: draw_area.h base_window.h log.h lua.h division.h map_natal.h
program.o: date.h timezone.h place.h map_progres.h map_develop.h map_direct.h
program.o: map_date_select.h map_multy.h map_eqlipse.h map_middle.h
program.o: map_harmonic.h map_ingres.h map_raw.h map_revolution.h
program.o: map_stationar.h map_aspect.h resolve_aspect.h static_holder.h
projection.o: projection.h astro.h program.h maps.h map.h point.h coord.h
projection.o: utils.h map_natal.h date.h timezone.h place.h draw_area.h lua.h
resolve_aspect.o: utils.h program.h astro.h maps.h map.h point.h coord.h
resolve_aspect.o: resolve_aspect.h date.h timezone.h lua.h
static_holder.o: program.h lua.h division.h
timezone.o: program.h astro.h maps.h map.h point.h coord.h utils.h timezone.h
transform_equator.o: map_natal.h map.h astro.h point.h coord.h utils.h date.h
transform_equator.o: timezone.h place.h transform_equator.h transform.h
transform_horizont.o: map_natal.h map.h astro.h point.h coord.h utils.h
transform_horizont.o: date.h timezone.h place.h transform_horizont.h
transform_horizont.o: transform.h
transform_inmundo.o: map_natal.h map.h astro.h point.h coord.h utils.h date.h
transform_inmundo.o: timezone.h place.h transform_inmundo.h transform.h
transform_sidereal.o: date.h astro.h timezone.h transform_sidereal.h
transform_sidereal.o: transform.h map.h point.h coord.h utils.h
utils.o: utils.h
window.o: window.h maps.h astro.h map.h point.h coord.h utils.h draw_area.h
window.o: base_window.h program.h projection.h aspects.h map_progres.h
window.o: map_develop.h map_natal.h date.h timezone.h place.h map_direct.h
window.o: map_eqlipse.h map_raw.h log.h lua.h lua_gui.h
