// =============================================================================
//
// Copyright (c) 2013 Christopher Baker <http://christopherbaker.net>
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


#pragma once


#include <set>
#include "ofx/HTTP/BaseRoute.h"
#include "ofx/HTTP/AbstractServerTypes.h"
#include "ofx/HTTP/WebSocketEvents.h"


namespace ofx {
namespace HTTP {


/// \brief Settings for a WebSocketRoute.
class WebSocketRouteSettings: public BaseRouteSettings
{
public:
    /// \brief A typedef for subprotocols.
    typedef std::set<std::string> SubprotocolSet;

    /// \brief A typedef for origins.
    typedef std::set<std::string> OriginSet;

    /// \brief Create a WebSocketRouteSettings.
    /// \param routePathPattern The regex pattern that this route will handle.
    /// \param requireSecurePort True if this route requires communication
    ///        on an SSL encrypted port.
    WebSocketRouteSettings(const std::string& routePathPattern = DEFAULT_WEBSOCKET_ROUTE_PATH_PATTERN,
                           bool requireSecurePort = false);

    /// \brief Destroy the WebSocketRouteSettings.
    virtual ~WebSocketRouteSettings();

    /// \brief Set the list of valid subprotocols.
    ///
    /// An empty set means that any requested subprotocol will be accepted.  A
    /// non-empty set means that the requested subprotocol MUST be in the set.
    ///
    /// \param subprotocols A set of valid subprotocols.
    void setValidSubprotcols(const SubprotocolSet& subprotocols);

    /// \brief Get a set of valid subprotocols.
    ///
    /// If empty, all requested subprotocols will be accepted.
    ///
    /// \returns The set of valid subprotocols.
    const SubprotocolSet& getValidSubprotocols() const;

    /// \brief Set the list of valid origins.
    ///
    /// An empty set means that any Origin header in the request will be
    /// accepted.  A non-empty set means that the request origin MUST be in the
    /// set.
    ///
    /// \param origins A set of valid origins.
    void setValidOrigins(const OriginSet& origins);

    /// \brief Get a set of valid origins.
    ///
    /// If empty, all Origin headers will be accepted.
    ///
    /// \returns The set of valid origins.
    const OriginSet& getValidOrigins() const;

    /// \param autoPingPongResponse If set to true, the WebSocket connection
    ///        will return all PINGs with a PONG.
    void setAutoPingPongResponse(bool autoPingPongResponse);

    bool getAutoPingPongResponse() const;

    void setKeepAlive(bool keepAlive);

    bool getKeepAlive() const;

    void setReceiveTimeout(const Poco::Timespan& receiveTimeout);

    Poco::Timespan getReceiveTimeout() const;

    void setSendTimeout(const Poco::Timespan& sendTimeout);

    Poco::Timespan getSendTimeout() const;

    void setPollTimeout(const Poco::Timespan& pollTimeout);

    Poco::Timespan getPollTimeout() const;

    /// \brief Set the WebSocket buffers size.
    /// \param bufferSize The buffer size in bytes.
    void setBufferSize(std::size_t bufferSize);

    /// \brief Get the WebSocket buffer size.
    /// \returns the WebSocket buffers size in bytes.
    std::size_t getBufferSize() const;

    /// \brief The default WebSocketRoute path pattern.
    static const std::string DEFAULT_WEBSOCKET_ROUTE_PATH_PATTERN;
    static const Poco::Timespan DEFAULT_RECEIVE_TIMEOUT;
    static const Poco::Timespan DEFAULT_SEND_TIMEOUT;
    static const Poco::Timespan DEFAULT_POLL_TIMEOUT;

    enum
    {
        /// \brief Default buffer size in bytes.
        DEFAULT_BUFFER_SIZE = 8192
    };
    
    
private:
    SubprotocolSet _validSubprotocols;
    OriginSet _validOrigins;
    
    /// \brief Automatically return pong frames.
    bool _autoPingPongResponse;
    bool _keepAlive;
    
    Poco::Timespan _receiveTimeout;
    Poco::Timespan _sendTimeout;
    Poco::Timespan _pollTimeout;
    
    /// \brief WebSocket buffer size in bytes.
    std::size_t _bufferSize;
    
};


/// \brief A route for handling WebSockets.
class WebSocketRoute: public BaseRoute_<WebSocketRouteSettings>
{
public:
    /// \brief A typedef for the WebSocketRouteSettings.
    typedef WebSocketRouteSettings Settings;

    /// \brief Create a WebSocketRoute with the given Settings.
    /// \param settings The WebSocketRoute settings.
    WebSocketRoute(const Settings& settings);

    /// \brief Destroy the WebSocketRoute.
    virtual ~WebSocketRoute();

    /// \brief A custom canHandleRequest extends the BaseRoute to check
    ///        for WebSocket upgrade headers.
    /// \param request The HTTPServerRequest to test.
    /// \param isSecurePort Indicates whether the request was passed on
    ///        an SSL encrypted port.
    /// \sa BaseRoute_<WebSocketRouteSettings>::canHandleRequest()
    virtual bool canHandleRequest(const Poco::Net::HTTPServerRequest& request,
                                  bool isSecurePort) const;

    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

    virtual void stop();

    bool sendFrame(const WebSocketConnection* connection,
                   const WebSocketFrame& frame);

    void broadcast(const WebSocketFrame& frame);

    void close(WebSocketConnection* connection);

    void close();

    template<class ListenerClass>
    void registerWebSocketEvents(ListenerClass* listener);

    template<class ListenerClass>
    void unregisterWebSocketEvents(ListenerClass* listener);

    std::size_t getNumWebSocketConnections() const;

    /// \brief WebSocketEvents for WebSocket callbacks.
    WebSocketEvents events;


protected:
    void registerWebSocketConnection(WebSocketConnection* connection);
    void unregisterWebSocketConnection(WebSocketConnection* connection);

    friend class WebSocketConnection;
    
private:
    typedef std::set<WebSocketConnection*> WebSocketConnections;
    typedef WebSocketConnections::iterator WebSocketConnectionsIter;

    WebSocketConnections _connections;

    mutable ofMutex _mutex; // locks the handlers set

};


template<class ListenerClass>
void WebSocketRoute::registerWebSocketEvents(ListenerClass* listener)
{
    ofAddListener(events.onOpenEvent, listener, &ListenerClass::onWebSocketOpenEvent);
    ofAddListener(events.onCloseEvent,listener, &ListenerClass::onWebSocketCloseEvent);
    ofAddListener(events.onFrameReceivedEvent, listener, &ListenerClass::onWebSocketFrameReceivedEvent);
    ofAddListener(events.onFrameSentEvent,listener,&ListenerClass::onWebSocketFrameSentEvent);
    ofAddListener(events.onErrorEvent,listener,&ListenerClass::onWebSocketErrorEvent);
}


template<class ListenerClass>
void WebSocketRoute::unregisterWebSocketEvents(ListenerClass* listener)
{
    ofRemoveListener(events.onOpenEvent,listener,&ListenerClass::onWebSocketOpenEvent);
    ofRemoveListener(events.onCloseEvent,listener,&ListenerClass::onWebSocketCloseEvent);
    ofRemoveListener(events.onFrameReceivedEvent,listener,&ListenerClass::onWebSocketFrameReceivedEvent);
    ofRemoveListener(events.onFrameSentEvent,listener,&ListenerClass::onWebSocketFrameSentEvent);
    ofRemoveListener(events.onErrorEvent,listener,&ListenerClass::onWebSocketErrorEvent);
}


} } // namespace ofx::HTTP
