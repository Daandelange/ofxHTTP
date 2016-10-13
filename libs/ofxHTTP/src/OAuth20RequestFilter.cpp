// =============================================================================
//
// Copyright (c) 2014-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofx/HTTP/OAuth20RequestFilter.h"
#include "Poco/Net/OAuth20Credentials.h"
#include "ofx/HTTP/BaseRequest.h"
#include "ofx/HTTP/HTTPUtils.h"


namespace ofx {
namespace HTTP {


OAuth20RequestFilter::OAuth20RequestFilter()
{
}


OAuth20RequestFilter::OAuth20RequestFilter(const OAuth20Credentials& credentials):
    _credentials(credentials)
{
}


void OAuth20RequestFilter::setCredentials(const OAuth20Credentials& credentials)
{
    _credentials = credentials;
}


OAuth20Credentials OAuth20RequestFilter::getCredentials() const
{
    return _credentials;
}


void OAuth20RequestFilter::requestFilter(Context& context,
                                         BaseRequest& request) const
{
    Poco::Net::OAuth20Credentials credentials(_credentials.getBearerToken(),
                                              _credentials.getScheme());

    credentials.authenticate(request);
}


} } // namespace ofx::OAuth
