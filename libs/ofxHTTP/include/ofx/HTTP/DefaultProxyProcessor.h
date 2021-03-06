//
// Copyright (c) 2013 Christopher Baker <http://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#pragma once


#include "ofx/HTTP/AbstractClientTypes.h"


namespace ofx {
namespace HTTP {


class DefaultProxyProcessor: public AbstractRequestResponseFilter
{
public:
    DefaultProxyProcessor();

    virtual ~DefaultProxyProcessor();

    virtual void requestFilter(Context& context, BaseRequest& request) const override;

    virtual void responseFilter(Context& context,
                                BaseRequest& request,
                                BaseResponse& response) const override;

};


} } // namespace ofx::HTTP
