/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * artnetplugin.cpp
 * The Art-Net plugin for lla
 * Copyright (C) 2005  Simon Newton
 */

#include <stdlib.h>
#include <stdio.h>

#include <llad/pluginadaptor.h>
#include <llad/preferences.h>

#include "artnetplugin.h"
#include "artnetdevice.h"


#define ARTNET_LONG_NAME	"lla - ArtNet node"
#define ARTNET_SHORT_NAME	"lla - ArtNet node"
#define ARTNET_SUBNET "0"

/*
 * Entry point to this plugin
 */
extern "C" Plugin* create(const PluginAdaptor *pa) {
  return new ArtNetPlugin(pa, LLA_PLUGIN_ARTNET);
}

/*
 * Called when the plugin is unloaded
 */
extern "C" void destroy(Plugin* plug) {
  delete plug;
}


/*
 * Start the plugin
 *
 * For now we just have one device.
 * TODO: allow multiple devices on different IPs ?
 */
int ArtNetPlugin::start() {
	int sd;
	
	if(m_enabled)
		return -1 ;
	
	// setup prefs
	m_prefs = load_prefs() ;

	if(m_prefs == NULL) 
		return -1 ;

	/* create new lla device */
	m_dev = new ArtNetDevice(this, "Art-Net Device", m_prefs) ;

	if(m_dev == NULL) 
		return -1  ;

	if(m_dev->start()) {
		delete m_dev ;
		return -1 ;
	}

	// register our descriptors, this should really be fatal for this plugin if it fails
	if ((sd = m_dev->get_sd()) >= 0)
		m_pa->register_fd( sd, PluginAdaptor::READ, m_dev) ;
	
	m_pa->register_device(m_dev) ;

	m_enabled = true ;
	return 0;
}


/*
 * Stop the plugin
 *
 * @return 0 on sucess, -1 on failure
 */
int ArtNetPlugin::stop() {
			
	if (!m_enabled)
		return -1 ;
	
	m_pa->unregister_fd( m_dev->get_sd(), PluginAdaptor::READ)  ;

	// stop the device
	if (m_dev->stop())
		return -1 ;
	

	m_pa->unregister_device(m_dev) ;
	m_enabled = false ;
	delete m_dev ;
	delete m_prefs ;

	return 0;
}

/*
 * return the description for this plugin
 *
 */
string ArtNetPlugin::get_desc() const {
		return
"ArtNet Plugin\n"
"----------------------------\n"
"\n"
"This plugin creates a single device with four input and four output ports.\n"
"\n"
"Art-Net has the concept of 'ports' on a device. Each device can support a maximum "
"of 4 ports in each direction and each port is assigned a universe address in "
"the range 0-255. When sending data from a (lla) port, the data is addressed to the "
"universe the (lla) port is patched to. For example if (lla) port 0 is patched "
"to universe 10, the data will be sent to Art-Net universe 10.\n"
"\n"
"--- Config file : lla-artnet.conf ---\n"
"\n"
"ip = a.b.c.d\n"
"The ip address to bind to. If not specified it will use the first non-loopback ip.\n"
"\n"
"long_name = lla - ArtNet node\n"
"The long name of the node.\n"
"\n"
"short_name = lla - ArtNet node\n"
"The short name of the node (first 17 chars will be used)\n"
"\n"
"subnet = 0\n"
"The ArtNet subnet to use (0-15).\n" ;
}



/*
 * load the plugin prefs and default to sensible values
 *
 */
Preferences *ArtNetPlugin::load_prefs() {
	Preferences *prefs = new Preferences("artnet") ;

	if(prefs == NULL)
		return NULL ;

	prefs->load() ;

	// we don't worry about ip here
	// if it's non existant it will choose one
	if( prefs->get_val("short_name") == "") {
		prefs->set_val("short_name",ARTNET_SHORT_NAME) ;
		prefs->save() ;
	}

	if( prefs->get_val("long_name") == "") {
		prefs->set_val("long_name",ARTNET_LONG_NAME) ;
		prefs->save() ;
	}

	if( prefs->get_val("subnet") == "") {
		prefs->set_val("subnet", ARTNET_SUBNET) ;
		prefs->save() ;
	}

	// check if this save correctly
	// we don't want to use it if null
	if( prefs->get_val("short_name") == "" ||
		prefs->get_val("long_name") == "" ||
		prefs->get_val("subnet") == "" ) {
		delete prefs;
		return NULL ;
	}

	return prefs ;
}
