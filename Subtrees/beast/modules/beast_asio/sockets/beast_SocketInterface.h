//------------------------------------------------------------------------------
/*
    This file is part of Beast: https://github.com/vinniefalco/Beast
    Copyright 2013, Vinnie Falco <vinnie.falco@gmail.com>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#ifndef BEAST_SOCKETINTERFACE_H_INCLUDED
#define BEAST_SOCKETINTERFACE_H_INCLUDED

/** These define the interfaces that SocketWrapper can adapt with SFINAE. */
struct SocketInterface
{
    /** Tag for some compatibility with asio::basic_socket
        http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/reference/basic_socket.html
    */
    struct Socket { };

    /** Tag for some compatibility with asio::basic_stream_socket
        http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/reference/basic_stream_socket.html
    */
    struct SyncStream { };
    struct AsyncStream { };
    struct Stream : SyncStream, AsyncStream { };

    /** Tags for compatibility with asio::ssl::stream
        http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/reference/ssl__stream.html
    */
    struct SyncHandshake { };
    struct SyncBufferedHandshake : SyncHandshake { };
    struct AsyncHandshake { };
    struct AsyncBufferedHandshake : AsyncHandshake{ };
    struct Handshake : SyncBufferedHandshake, AsyncBufferedHandshake { };

protected:
    //--------------------------------------------------------------------------

    /** Template specialization to determine available interfaces. */
    template <typename Object>
    struct InterfacesOf
    {
        /** Intrusive tag support.

            To use this, add a struct called SocketInterfaces to your
            class and derive it from the interfaces that you support.
            For example:

            @code

            struct MyHandshakingStream
            {
                struct SocketInterfaces
                    : SocketInterface::Stream
                    , SocketInterface::Handshake
                {
                };
            }

            @endcode
        */
        typedef typename Object::SocketInterfaces type;
        typedef type value;
    };

    // Specialization for boost::asio::basic_socket
    template <typename Protocol, typename SocketService>
    struct InterfacesOf <boost::asio::basic_socket <Protocol, SocketService> >
    {
        struct value : SocketInterface::Socket { };
        typedef value type;
    };

    // Specialization for boost::asio::basic_stream_socket
    template <typename Protocol, typename SocketService>
    struct InterfacesOf <boost::asio::basic_stream_socket <Protocol, SocketService> >
    {
        struct value : SocketInterface::Socket, SocketInterface::Stream { };
        typedef value type;
    };

    // Specialization for boost::asio::ssl::stream
    template <typename Stream>
    struct InterfacesOf <boost::asio::ssl::stream <Stream> >
    {
        struct value : SocketInterface::Stream , SocketInterface::Handshake { };
        typedef value type;
    };

#if 1
    // Less elegant, but works.
    // Determines if Object supports the specified Interface
    template <typename Object, typename Interface, class Enable = void>
    struct HasInterface : boost::false_type { };

    template <typename Object, typename Interface>
    struct HasInterface <Object, Interface,
        typename boost::enable_if <boost::is_base_of <
        Interface, typename InterfacesOf <Object>::type> >::type >
        : boost::true_type { };
#else
    // This should work, but doesn't.
    // K-ballo from #boost suggested it.
    //
    // Determines if Object supports the specified Interface
    template <typename Object, typename Interface>
    struct HasInterface : boost::is_base_of <Interface, typename InterfacesOf <Object> >
    {
    };
#endif
};

#endif
