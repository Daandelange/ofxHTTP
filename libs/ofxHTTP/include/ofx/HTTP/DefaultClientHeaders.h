//
// Copyright (c) 2013 Christopher Baker <http://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#pragma once


#include "ofx/HTTP/AbstractClientTypes.h"


namespace ofx {
namespace HTTP {


class DefaultClientHeaders: public AbstractRequestFilter
{
public:
    DefaultClientHeaders();

    virtual ~DefaultClientHeaders();

    virtual void requestFilter(Context& context, BaseRequest& request) const override;

};


} } // namespace ofx::HTTP
