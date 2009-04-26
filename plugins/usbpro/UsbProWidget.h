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
 * UsbProWidget.h
 * Interface for the usbpro device
 * Copyright (C) 2006  Simon Newton
 */

#ifndef USBPROWIDGET_H
#define USBPROWIDGET_H

#include <string>
#include <stdint.h>
#include <lla/select_server/Socket.h>

#include "UsbProWidgetListener.h"

namespace lla {
namespace plugin {

using std::string;
using lla::select_server::ConnectedSocket;

enum { DMX_BUF_LEN = 513 };
enum { USER_CONFIG_LEN = 508 };

// dmx message
typedef struct {
  uint8_t dmx[DMX_BUF_LEN];
} pms_dmx;

//
typedef struct {
  uint8_t status;
  uint8_t dmx[DMX_BUF_LEN];
} pms_rdmx;

//
typedef struct {
  uint8_t dmx[DMX_BUF_LEN];
} pms_rdm;

// param request
typedef struct {
  uint8_t len;
  uint8_t len_hi;
} pms_prmreq;


// parameters
typedef struct {
  uint8_t firmv;
  uint8_t firmv_hi;
  uint8_t brtm;
  uint8_t mabtm;
  uint8_t rate;
} pms_parameters;

// param reply
typedef struct {
  pms_parameters base_parameters;
  uint8_t user[USER_CONFIG_LEN];
} pms_prmrep;

// param set
typedef struct {
  uint8_t len;
  uint8_t len_hi;
  uint8_t brk;
  uint8_t mab;
  uint8_t rate;
  uint8_t user[USER_CONFIG_LEN];
} pms_prmset;

// change recv mode
typedef struct {
  uint8_t  mode;
} pms_rcmode;

// change of state message
typedef struct {
  uint8_t  start;
  uint8_t  changed[5];
  uint8_t data[40];
} pms_cos;

// serial number request
struct pms_snoreq_s {
  uint8_t i[];    // required for a struct size of 0
}__attribute__( ( packed ) );

typedef struct pms_snoreq_s pms_snoreq;

// serial number reply
typedef struct {
  uint8_t srno[SERIAL_NUMBER_LENGTH];
} pms_snorep;

// union of all messages
typedef union {
    pms_dmx     pmu_dmx;
    pms_rdmx     pmu_rdmx;
    pms_rdm     pmu_rdm;
    pms_prmreq     pmu_prmreq;
    pms_prmrep    pmu_prmrep;
    pms_prmset    pmu_prmset;
    pms_rcmode    pmu_rcmode;
    pms_cos      pmu_cos;
    pms_snoreq    pmu_snoreq;
    pms_snorep    pmu_snorep;
} pmu;

// the entire message
typedef struct {
  uint8_t som;
  uint8_t label;
  uint8_t len;
  uint8_t len_hi;
  pmu pm_pmu;
} promsg;

#define pm_dmx    pm_pmu.pmu_dmx
#define pm_rdmx    pm_pmu.pmu_rdmx
#define pm_rdm    pm_pmu.pmu_rdm
#define pm_prmreq  pm_pmu.pmu_prmreq
#define pm_prmrep  pm_pmu.pmu_prmrep
#define pm_prmset  pm_pmu.pmu_prmset
#define pm_rcmode  pm_pmu.pmu_rcmode
#define pm_cos    pm_pmu.pmu_cos
#define pm_snoreq  pm_pmu.pmu_snoreq
#define pm_snorep  pm_pmu.pmu_snorep


class UsbProWidget: public lla::select_server::SocketListener {
  public:
    UsbProWidget():
      m_enabled(false),
      m_break_time(K_MISSING_PARAM),
      m_mab_time(K_MISSING_PARAM),
      m_rate(K_MISSING_PARAM),
      m_socket(NULL) {}
    ~UsbProWidget() {}

    bool Connect(const string &path);
    bool Disconnect();
    ConnectedSocket *GetSocket() { return m_socket; }

    bool SendDmx(const uint8_t *buf, unsigned int len) const;
    bool SendRdm(const uint8_t *buf, unsigned int len) const;
    bool GetParameters() const;
    bool GetSerial() const;
    bool SetParameters(uint8_t *data,
                       unsigned int len,
                       int brk,
                       int mab,
                       int rate);
    int FetchDmx(uint8_t *data, unsigned int len) const;

    bool ChangeToReceiveMode();
    void SetListener(UsbProWidgetListener *listener);
    int SocketReady(ConnectedSocket *socket);

  private:
    bool SendMessage(promsg *msg) const;
    int set_msg_len(promsg *msg, int len) const;
    bool SendChangeMode(int mode);
    int handle_dmx(pms_rdmx *dmx, int len);
    int handle_cos(pms_cos *cos, int len);
    int handle_prmrep(pms_prmrep *rep, unsigned int len);
    int handle_snorep(pms_snorep *rep, int len);
    int ReceiveMessage();

    uint8_t m_dmx[DMX_BUF_LEN - 1];  // dmx buffer
    bool m_enabled;
    uint8_t m_break_time;
    uint8_t m_mab_time;
    uint8_t m_rate;
    UsbProWidgetListener *m_listener;
    ConnectedSocket *m_socket;
    static const int K_MISSING_PARAM = -1;
    static const int K_HEADER_SIZE = 4;
    static const uint8_t K_START_CODE = 0;
};

} // plugin
} //lla
#endif
