//
// Copyright (c) 2013 Christopher Baker <http://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofx/HTTP/GetRequestTask.h"
#include "ofx/HTTP/GetRequest.h"
#include "ofx/MediaTypeMap.h"


namespace ofx {
namespace HTTP {


GetRequestTask::GetRequestTask(const std::string& uri, const std::string& cachePath):
    Poco::Task(uri),
    _uri(uri),
    _cachePath(cachePath)
{
}


GetRequestTask::~GetRequestTask()
{
}


void GetRequestTask::runTask()
{
    Client client;
    Context context;
    GetRequest request(_uri);
    auto l0 = context.events.onHTTPClientStateChange.newListener(this, &GetRequestTask::onHTTPClientStateChange);
    auto l1 = context.events.onHTTPClientRequestProgress.newListener(this, &GetRequestTask::onHTTPClientRequestProgress);
    auto l2 = context.events.onHTTPClientResponseProgress.newListener(this, &GetRequestTask::onHTTPClientResponseProgress);
    auto l3 = context.events.onHTTPClientResponseStream.newListener(this, &GetRequestTask::onHTTPClientResponseStream);
    auto l4 = context.events.onHTTPClientError.newListener(this, &GetRequestTask::onHTTPClientError);
    client.submit(context, request);
}


bool GetRequestTask::onHTTPClientStateChange(ClientStateChangeEventArgs& args)
{
    return true;
}


bool GetRequestTask::onHTTPClientRequestProgress(ClientRequestProgressEventArgs& args)
{
    float progress = args.progress().progress();

    if (progress == Poco::Net::HTTPMessage::UNKNOWN_CONTENT_LENGTH)
    {
        setProgress(0);
    }
    else
    {
        setProgress(progress);
    }

    return true;
}


bool GetRequestTask::onHTTPClientResponseProgress(ClientResponseProgressEventArgs& args)
{
    float progress = args.progress().progress();

    if (progress == Poco::Net::HTTPMessage::UNKNOWN_CONTENT_LENGTH)
    {
        setProgress(0);
    }
    else
    {
        setProgress(progress);
    }

    return true;
}


bool GetRequestTask::onHTTPClientResponseStream(ClientResponseStreamEventArgs& args)
{
    ofBuffer buffer(args.stream());

    if (!_cachePath.empty())
    {
        ofBufferToFile(_cachePath, buffer);
    }

    Poco::Net::MediaType mediaType(args.response().getContentType());

    if (mediaType.matches("image"))
    {
        ofPixels pixels;
        ofLoadImage(pixels, buffer);
        postNotification(new Poco::TaskCustomNotification<ofPixels>(this, pixels));
    }
    else if (mediaType.matches("application", "json"))
    {
        ofJson json = ofJson::parse(buffer);
        postNotification(new Poco::TaskCustomNotification<ofJson>(this, json));
    }
    else
    {
        std::cout << "Unknown type " << mediaType.toString() << " posting buffer.";
        postNotification(new Poco::TaskCustomNotification<ofBuffer>(this, buffer));
    }

    return true;
}


bool GetRequestTask::onHTTPClientError(ClientErrorEventArgs& args)
{
    // We throw an exception to be caught by the task manager.
    throw args.exception();

    // Unreachable.
    return true;
}


} } // namespace ofx::HTTP