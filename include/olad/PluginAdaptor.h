/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * PluginAdaptor.h
 * The provides operations on a ola_device.
 * Copyright (C) 2005 Simon Newton
 */

/**
 * @file PluginAdaptor.h
 * @brief Provides a wrapper for the DeviceManager and SelectServer objects so
 * that the plugins can register devices and file handles for events
 */

#ifndef INCLUDE_OLAD_PLUGINADAPTOR_H_
#define INCLUDE_OLAD_PLUGINADAPTOR_H_

#include <ola/Callback.h>
#include <ola/Clock.h>
#include <ola/ExportMap.h>
#include <ola/base/Macro.h>
#include <ola/io/SelectServerInterface.h>

#include <string>

namespace ola {

class PluginAdaptor: public ola::io::SelectServerInterface {
 public:
  /**
   * @brief Create a new PluginAdaptor
   * @param device_manager  pointer to a DeviceManager object
   * @param select_server pointer to the SelectServer object
   * @param export_map pointer to the ExportMap object
   * @param preferences_factory pointer to the PreferencesFactory object
   * @param port_broker pointer to the PortBroker object
   */
  PluginAdaptor(class DeviceManager *device_manager,
                ola::io::SelectServerInterface *select_server,
                ExportMap *export_map,
                class PreferencesFactory *preferences_factory,
                class PortBrokerInterface *port_broker);

  // The following methods are part of the SelectServerInterface
  /**
   * @brief Register a descriptor with the select server.
   * @param descriptor the descriptor to register
   * @return true on success, false on failure.
   */
  bool AddReadDescriptor(ola::io::ReadFileDescriptor *descriptor);

  /**
   * @brief Register a descriptor with the select server.
   * @param descriptor the descriptor to register
   * @param delete_on_close if true, ownership of the
   * ola::io::ConnectedDescriptor is transferred to the SelectServer
   * @return true on success, false on failure.
   */
  bool AddReadDescriptor(ola::io::ConnectedDescriptor *descriptor,
                         bool delete_on_close = false);

  /**
   * @brief Remove a descriptor from the select server
   */
  void RemoveReadDescriptor(ola::io::ReadFileDescriptor *descriptor);

  /**
   * @brief Remove a descriptor from the select server
   */
  void RemoveReadDescriptor(ola::io::ConnectedDescriptor *descriptor);

  /**
   * @brief Register a WriteFileDescriptor with the SelectServer
   */
  bool AddWriteDescriptor(ola::io::WriteFileDescriptor *descriptor);

  /**
   * @brief Remove a descriptor from the select server
   */
  void RemoveWriteDescriptor(ola::io::WriteFileDescriptor *descriptor);

  /**
   * @brief Register a repeating timeout
   * @param ms the time between function calls
   * @param closure the OlaClosure to call when the timeout expires
   * @return a timeout_id on success or K_INVALID_TIMEOUT on failure
   */
  ola::thread::timeout_id RegisterRepeatingTimeout(unsigned int ms,
                                                   Callback0<bool> *closure);

  /**
   * Register a repeating timeout
   * @param interval the time between function calls
   * @param closure the OlaClosure to call when the timeout expires
   * @return a timeout_id on success or K_INVALID_TIMEOUT on failure
   */
  ola::thread::timeout_id RegisterRepeatingTimeout(
      const TimeInterval &interval,
      Callback0<bool> *closure);

  /**
   * @brief Register a single timeout
   * @param ms the time between function calls
   * @param closure the OlaClosure to call when the timeout expires
   * @return a timeout_id on success or K_INVALID_TIMEOUT on failure
   */
  ola::thread::timeout_id RegisterSingleTimeout(
      unsigned int ms,
      SingleUseCallback0<void> *closure);

  /**
   * @brief Register a single timeout
   * @param interval the time between function calls
   * @param closure the OlaClosure to call when the timeout expires
   * @return a timeout_id on success or K_INVALID_TIMEOUT on failure
   */
  ola::thread::timeout_id RegisterSingleTimeout(
      const TimeInterval &interval,
      SingleUseCallback0<void> *closure);

  /*
   * @brief Remove a timeout
   * @param id the id of the timeout to remove
   */
  void RemoveTimeout(ola::thread::timeout_id id);

  /**
   * @brief Execute a closure in the main thread.
   * @param closure the closure to execute.
   */
  void Execute(ola::BaseCallback0<void> *closure);

  /**
   * @brief Return the wake up time for the select server
   * @return a TimeStamp object
   */
  const TimeStamp *WakeUpTime() const;

  ExportMap *GetExportMap() const {
    return m_export_map;
  }

  // These are the extra bits for the plugins
  /**
   * @brief Register a device
   * @param device  the device to register
   * @return true on success, false on error
   */
  bool RegisterDevice(class AbstractDevice *device) const;

  /**
   * @brief Unregister a device
   * @param device  the device to unregister
   * @return true on success, false on error
   */
  bool UnregisterDevice(class AbstractDevice *device) const;

  /**
   * @brief Create a new preferences container
   * @return a Preferences object
   */
  class Preferences *NewPreference(const std::string &name) const;
  class PortBrokerInterface *GetPortBroker() const {
    return m_port_broker;
  }

 private:
  DeviceManager *m_device_manager;
  ola::io::SelectServerInterface *m_ss;
  ExportMap *m_export_map;
  class PreferencesFactory *m_preferences_factory;
  class PortBrokerInterface *m_port_broker;

  DISALLOW_COPY_AND_ASSIGN(PluginAdaptor);
};
}  // namespace ola
#endif  // INCLUDE_OLAD_PLUGINADAPTOR_H_
