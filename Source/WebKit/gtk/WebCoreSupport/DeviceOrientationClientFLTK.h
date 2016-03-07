/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011 Collabora Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */
#ifndef DeviceOrientationClientFLTK_h
#define DeviceOrientationClientFLTK_h

#include "DeviceOrientationClient.h"
#include "DeviceOrientationData.h"

namespace WebKit {

class DeviceOrientationClientFLTK : public WebCore::DeviceOrientationClient {
public:
	DeviceOrientationClientFLTK();
	virtual ~DeviceOrientationClientFLTK();

	virtual void setController(WebCore::DeviceOrientationController*);
	virtual void startUpdating();
	virtual void stopUpdating();
	virtual WebCore::DeviceOrientationData* lastOrientation() const;
	virtual void deviceOrientationControllerDestroyed();

private:
	WebCore::DeviceOrientationController* m_controller;
};

} // namespace WebKit

#endif // DeviceOrientationClientFLTK_h