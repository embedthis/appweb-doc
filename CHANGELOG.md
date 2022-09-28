# Changelog

## [v8.3.0] - 2022-03-31

# Minor Patch Release

### Recommended Action

- [ ]  Optional Upgrade -- Upgrade only if convenient
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Essential Upgrade -- All users strongly advised to upgrade

### Change Log

* Fix HTTP/2 premature parsing of headers for requests with multiple header continuations
* Fix memory growth issue with systems that have very fast networks and slower CPUs
* Added HTTP_CURRENT flag to httpServiceNetQueues to service only pre-existing scheduled queues
* Change cookie parsing code to be more spec compliant
* Fix Proxy handler transfer of large files
* Update MbedTLS to version 2.28.0
* Fix building with OpenSSL crypto engines
* Fix errorDoc propagation to subsequent requests with HTTP/1
* Optimize pipeline processing and queue servicing
* Refactor tail filter and http/2 filter overlap
* Add HTTP_NET_IO event for Net callbacks.
* Add missing HTTP status codes
* Length check outsized upload file boundary string
* Fix missing host header when no host route name defined
* Fix rare condition where upload files prepend CRLF to uploaded file
* Fix proxy death handling
* Ensure disconnectStreams does immediate socket disconnect in HTTP/1
* Convert HTTP_ABORT to HTTP_CLOSE in httpError where possible.
* Fix proxy handling when networks disconnected
* Fix incoming chunked transfer processing with packet loss
* Increase proxy timeout defaults
* Fix premature request timeout of static files on very slow networks

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.3.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v8.2.5] - 2021-11-08

# Minor Patch Release

### Recommended Action

- [ ]  Optional Upgrade -- Upgrade only if convenient
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Essential Upgrade -- All users strongly advised to upgrade

### Change Log

* Fix ranged requests for errors
* Fix interrupted large file transfer with non-simple pipeline. #662
* Fix removed queue detection (changed for errored ranged requests)
* Fix unit test completion
* Increase default max streams per HTTP/2 connection
* Add license copyrights

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.2.5)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v8.2.4] - 2021-11-03

# Minor Patch Release

### Recommended Action

- [ ]  Optional Upgrade -- Upgrade only if convenient
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Essential Upgrade -- All users strongly advised to upgrade

### Change Log

* Fix proxy handler processing of Connection header
* Fix building on windows with ATOMIC APIs
* Fix building without fast/proxy on windows
* Fix a few minor windows compiler warnings

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.2.4)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v8.2.3] - 2021-09-08

# Minor Patch Release

### Recommended Action

- [ ]  Optional Upgrade -- Upgrade only if convenient
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Essential Upgrade -- All users strongly advised to upgrade

### Change Log

* Fix httpStealSocket (deprecated API) to adjust limit for per-client connections.
* Update some API stability classifications.
* Add constant time password comparison for basic/digest auth.
* Service queues before ready event so outstanding incoming queues are serviced first.
* Fix proxyHandler removing notifier when request is complete
* Remove testHandler in default build
* Fix parsing config inside disabled conditional directives
* Fix release of event object in mprCreateEvent

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.2.3)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v8.2.2] - 2021-08-04

# Minor Patch Release

### Recommended Action

- [ ]  Optional Upgrade -- Upgrade only if convenient
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Essential Upgrade -- All users strongly advised to upgrade

### Change Log

* Fix HTTP/2 compliance with h2spec test suite.
* Fix file uploadFilter CPU spin.
* Fix a few compiler warnings on newer compilers.
* Improve trace queues for debugging
* Fix URLs encoded with %00. #654
* Fix SIGINFO definitions #655
* Fix errorv stream guard #656  

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.2.2)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v8.2.1] - 2020-11-27

# Minor Release

### Recommended Action
- [x]  Optional Upgrade -- Upgrade only if convenient
- [ ]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Essential Upgrade -- All users strongly advised to upgrade

### Change Log

* Removed samples into separate repository
* Fix HTTP/1.0 connection recycling
* Fixes for proxy handler

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.2.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v8.2.0] - 2020-11-08

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

## Major New Features

* Proxy support (beta)
* FastCGI for faster execution of CGI programs and PHP (beta)
* Network connection notifier
* Optimizations up to 15% faster than Appweb 8

## Breaking Changes

Appweb 9 is API compatible with Appweb 8. It is more rigorous in its request pipeline processor so applications and handlers that took advantage of undocumented behavior may need to be modified.

### Potentially Breaking Changes

* The httpProcess() function is removed. Handlers and filters should advance state via the httpSetState and httpFinalize APIs.
* httpSetState now invokes the http pipeline and the state may advance before returning.
* httpFinalize, httpFinalizeOutput and httpFinalizeInput may invoke httpSetState and invoke the pipeline processor and advance state.
* The WebSockets last packet flag has been renamed from HttpPacket.last to HttpPacket.fin to avoid confusion / overload with HTTP/2. The "last" bit is set in a packet to indicate the last message in a HTTP/2 packet.
* The upload filter is now configurable per route.

### Change Log

* Add Proxy handler (beta)
* Add pretty request trace formatter
* Add CharSet directive to set Content-Type character set
* Add StreamInput wildcard
* Add HttpNet callback
* Add Listen [multiple] for port-reuse
* Add HttpUser.data field for user data
* Improve CanonicalName handling to apply per route
* Improved httpLog (trace) API with simplified formatting via the pretty formatter
* Make development countermeasures optional
* Make UploadFilter apply per route
* Optimize and refactor dispatch / event handling
* Optimize HTTP pipeline handling
* Optimizations for the MPR and event handling
* Fix LimitProcess off by one
* Fixes for FastCGI handler
* Fix LimitConnections and LimitConnectionsPerClient
* Fix case-insensitive routing
* Fix monitor IP address pruning
* Improve socket error log reporting
* Some HTTP/2 corner case fixes


### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.2.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v8.1.3] - 2020-10-15

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Fix tracking last net activity
* Add mime-type for error docs
* Fix upload data parse error
* Remove URI decode on upload form fields
* Fix status bit fields in FastCGI handler

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.1.3)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v8.1.1] - 2020-08-30

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Fix consuming blank lines with invalid HTTP white space in HTTP/1

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.1.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v8.1.0] - 2020-08-28

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Add FastCGI handler
* Fix ESP Sqlite grid paging
* Fix Sqlite multithread locking with ESP
* Fix SSLProtocol directive parsing
* Fix name clashes in combo builds
* Fix date validation in date parser
* Support OpenSSL certificates in DER format
* Deprecate in-memory PHP module in favor of FastCGI
* Update documentation
* Fix handling of invalid ranged requests
* Fix HTTP/2 huff decode error
* Fix HttpRx.bytesRead
* Fix HttpQueue queue pair stream reference
* Fix same-site cookie 'none' handling
* Add "abort" memory policy
* Add SSL preloading
* Fix request routing token expansion
* Fix digest authentication nonce handling
* Add select over pipes event handling
* Fix building on Arm 64 __aarch64__
* Fix memory leak for events on destroyed dispatchers
* Fix monitor counters missing reductions
* Fix connectionsPerClientMax default value
* Fix makefile generation missing mpr-version
* Fix LimitConnections calculations
* Remove Connection and Keep-Alive headers in HTTP/2
* Fix EVENT_DESTROY in HTTP/1

### Build Requirements
- To build requires MakeMe 1.0 or later
- To install packages, use Pak 1.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A8.1.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v8.0.0] - 2019-11-22

# Major Release

The source code for Appweb 8.X Enterprise Edition is available on request for commercial licensees. Contact dev@embedthis.com for details.

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [X]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* HTTP/2 protocol support
* Improve HTTP header and token parsing

### Build Requirements
- To build, requires MakeMe 1.0.2 or later
- To install packages, use Pak 1.0.3 or later

### See
- [Issues](https://github.com/embedthis/appweb-ee/issues?q=milestone%3A8.0.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v7.2.3] - 2020-08-28

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Optimize SSL retries
* Fix http digest nonce lifespan

### Build Requirements
- To build, requires MakeMe 1.0 or later
- To install packages, use Pak 1.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A7.2.3)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v7.2.1] - 2019-11-22

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Update samples
* Fix log file after backup
* Improve BSD building
* Add %f for formatting time in milliseconds
* Revise API stability classifications
* Fix TLSv1 enable / disable
* Add scaselesscontains API
* Improve combo builds
* Improve compiler API prototypes

### Build Requirements
- To build, requires MakeMe 1.0 or later
- To install packages, use Pak 1.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A7.2.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v7.2.0] - 2019-08-25

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Add new httpCreateEvent to simplify foreign event invocation
* Improve HTTP header and token parsing
* Fix file upload maximum form limit
* Reduce GC memory footprint
* Various GC improvements
* Fix ESP overwriting existing run targets

### Build Requirements
- To build, requires MakeMe 1.0 or later
- To install packages, use Pak 1.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A7.2.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v7.1.0] - 2018-12-14

# Minor Release

### Recommended Action
- [x]  Essential Upgrade -- All users strongly advised to upgrade
- [ ]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Support both OpenSSL 1.0 and 1.1 streams
* Improve windows building
* Support VS 2017 professional and community editions
* Improve function(void) prototypes
* Fix: palloc memory allocation race
* Fix compilation warnings
* Add community edition / enterprise edition license info
* Add some defensive argument checking

### Build Requirements
- To build, requires MakeMe 1.0 or later
- To install packages, use Pak 1.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A7.1.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v7.0.3] - 2018-04-17

# Security Patch Release

### Recommended Action
- [x]  Essential Upgrade -- All users strongly advised to upgrade
- [ ]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Fix basic/digest auth bypass #610
- Upgrade to match new Pak release (0.12.4) and using pak.json instead of package.json
- Update for OpenSSL 1.1.0, Mbedtls 2.8.0, Sqlite 3.23.1
- Update for Alpine linux

### Build Requirements
- To build, requires MakeMe 0.10.8 or later
- To install packages, use Pak 0.12.4 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A7.0.3)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v7.0.2] - 2018-02-13

# Security Patch Release

### Recommended Action
- [x]  Essential Upgrade -- All users strongly advised to upgrade
- [ ]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Fix denial of service #605

### Build Requirements
- To build, requires MakeMe 0.10.7 or later
- To install packages, use Pak 0.12.4 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A7.0.2)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v7.0.1] - 2017-09-28

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Improve Visual Studio 2017 support
- Cleanup old code

### Build Requirements
- To build, requires MakeMe 0.10.7 or later
- To install packages, use Pak 0.12.4 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A7.0.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v7.0.0] - 2017-09-28

# Major Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Visual Studio 2017 support
- Remove deprecated code

### Build Requirements
- To build, requires MakeMe 0.10.7 or later
- To install packages, use Pak 0.12.4 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A7.0.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v6.3.0] - 2019-08-13

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Improve HTTP header and token parsing

### Build Requirements
- To build, requires MakeMe 1.0 or later
- To install packages, use Pak 1.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.3.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v6.2.3] - 2016-06-16

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Add support for new build farm
- Fix compiler warnings
- Update MPR & HTTP
- Fix MPR cache key pruning

### Build Requirements
- To build, requires MakeMe 0.10.4 or later
- To install packages, use Pak 0.12.1 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.2.3)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v6.2.2] - 2016-06-02

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Upgrade to mbedtls 2.2.1
- Fix importing openssl libraries when building
- Add openssl renegotiation controls via main.me: ssl.handshakes
- Add issue template
- Add VxWorks 7 support
- Fix cross-compiling with sqlite support for esp
- Fix cross compiling sleuthing the cross compiler options
- Fix using --set compiler.has\* configuration options
- Added fortification to compiler options and ASLR

### Build Requirements
- To build, requires MakeMe 0.10.4 or later
- To install packages, use Pak 0.12.1 or later

### See
- [Includes 5.6.2 Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.6.2)
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.2.2)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v6.2.1] - 2015-12-28

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Fix JSON line number tracking for error reporting
- Fix openssl support not clearing the buffered read data flag - causing high CPU usage

### Build Requirements
- To build, requires MakeMe 0.10.0 or later
- To install packages, use Pak 0.12.0 or later

### See
- [Includes 5.6.0 Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.6.1)
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.2.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v6.2.0] - 2015-11-30

# Minor Feature Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Compatibility

ESP applications now require an **esp.app** and **http.pipeline** properties in their esp.json configuration file. This is to support optional loading esp.json files for stand-alone ESP pages. Previously, the presence of an esp.json file was used as an indicator that the ESP directory contained an ESP application.  The **http.pipeline** property configures the espHandler to serve all requests to the ESP application.

```
esp: {
    app: true
},
http: {
    pipeline: {
        handlers: 'espHandler',
    }
}
```

#### ServerName

The optional ServerName appweb.conf directive defines the host names that will be accepted in client requests. In this version, the ServerName is strictly observed and requests for other host names will not be served.

In legacy Appweb versions, the ServerName also defined the canonical host name to use in redirection responses. This role was replaced by the CanonicalName directive, but the ServerName was only advisory in that requests for non-specified hosts would be served via the default host configuration. In this release, the ServerName directive is strictly enforced and Appweb will only respond to host names specified by the ServerName directive. If a ServerName directive is not specified, then all host names will be served. If a ServerName is specified, then only those host names will be served. If you are wanting to define a canonical name for redirections, use CanonicalName and not ServerName.

### Change Log
- Support loading esp.json files for stand-alone ESP pages
- Improve esp loading diagnostics
- Add **esp.app** and **http.pipeline** configuration properties
- Simplify ESP loading code
- Optimize loading ESP applications

### Build Requirements
- To build, requires MakeMe 0.10.0 or later
- To install packages, use Pak 0.12.0 or later

### See
- [Includes 5.6.0 Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.6.0)
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.2.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v6.1.1] - 2015-10-28

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Build Requirements
- To build, requires MakeMe 0.10.0 or later
- To install packages, use Pak 0.12.0 or later

### See
- [Includes 5.5.1 Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.5.1)
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.1.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v6.1.0] - 2015-09-23

# Minor Feature Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Add MbedTLS support
- Fix session cookie handling
- Fix mprSort
- Fix ESP compile mixed modes
- Fix SSL write failure with pipelined requests
- Optimize mprCreateEvent and replace mprCreateEventOutside
- Fix ESP ediSortGrid to handle numeric columns
- Fix various error return paths
- Ignore requests when shutting down
- Ignore disabled monitors
- Support expanding request vars in HTTP header definitions

### Build Requirements
- To build, requires MakeMe 0.10.0 or later
- To install packages, use Pak 0.12.0 or later

### See
- [Includes 5.5.0 Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.5.0)
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.1.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v6.0.3] - 2015-08-26

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Updated samples
- Added configure.bat for windows
- Fixed building with VS 2015
- Unit test updates

### Build Requirements
- To build, requires MakeMe 0.9.3 or later
- To install paks, use Pak 0.11.3 or later

### See
- [Includes 5.4.7 Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.7)
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.0.3)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v6.0.2] - 2015-08-13

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Upgrade MPR, HTTP and ESP with fixes and enhancements

### Build Requirements
- To build, requires MakeMe 0.9.2 or later
- To install paks, use Pak 0.11.3 or later

### See
- [Includes 5.4.6 Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.6)
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.0.2)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)



## [v6.0.1] - 2015-07-20

# Minor Patch Release

## Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Fix ESP compiling applications in release mode
- Fix espMail failing to send email
- Fix formatting JSON strings with embedded non-alnum characters
- Move ESP db/migrations up one level

#### Build Requirements
- To build, requires MakeMe 0.9.1 or later
- To install paks, use Pak 0.11.2 or later

#### See
- [Includes 5.4.5 Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.5)
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.0.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v6.0.0] - 2015-06-18

### Major Feature Release
- This is the start of the 6.X stream. It initially has no changes from 5.4.4

### Build Requirements
- To build, requires MakeMe 0.9.0 or later
- To install paks, use Pak 0.11.1 or later

See:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A6.0.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v5.6.3] - 2018-12-23

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Improve HTTP header and token parsing

### Build Requirements
- To build, requires MakeMe 1.0 or later
- To install packages, use Pak 1.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.6.3)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v5.6.2] - 2018-04-14

# Minor Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log

* Improve HTTP header and token parsing
### Build Requirements
- To build, requires MakeMe 1.0 or later
- To install packages, use Pak 1.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.6.2)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v5.6.1] - 2015-12-28

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Fix JSON line number tracking for error reporting
- Fix openssl support not clearing the buffered read data flag - causing high CPU usage

### Build Requirements
- To build, requires MakeMe 0.10.0 or later
- To install packages, use Pak 0.12.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.6.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v5.6.0] - 2015-11-30

# Minor Feature Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Compatibility

ESP applications now require an **esp.app** and **http.pipeline** properties in their esp.json configuration file. This is to support optional loading esp.json files for stand-alone ESP pages. Previously, the presence of an esp.json file was used as an indicator that the ESP directory contained an ESP application.  The **http.pipeline** property configures the espHandler to serve all requests to the ESP application.

```
esp: {
    app: true
},
http: {
    pipeline: {
        handlers: 'espHandler',
    }
}
```

#### ServerName

The optional ServerName appweb.conf directive defines the host names that will be accepted in client requests. In this version, the ServerName is strictly observed and requests for other host names will not be served.

In legacy Appweb versions, the ServerName also defined the canonical host name to use in redirection responses. This role was replaced by the CanonicalName directive, but the ServerName was only advisory in that requests for non-specified hosts would be served via the default host configuration. In this release, the ServerName directive is strictly enforced and Appweb will only respond to host names specified by the ServerName directive. If a ServerName directive is not specified, then all host names will be served. If a ServerName is specified, then only those host names will be served. If you are wanting to define a canonical name for redirections, use CanonicalName and not ServerName.

### Change Log
- Support loading esp.json files for stand-alone ESP pages
- Improve esp loading diagnostics
- Add **esp.app** and **http.pipeline** configuration properties
- Fix cross-compiling ESP applications and support --static and --rom
- Fix CPU hog with certain invalid HTTP requests
- Add support for SSL SNI based connections
- Fix ESP range requests
- Change ShowDefaults to be false by default
- Fix ESP in ROM mode
- Add esp --combine option for compiling for ROM
- Fix building without a file system (ROM) support
- Enable ROM support with disk file system support. See issue #550
- Makeme command updated with new options
- Fix retrying client requests failing cert validation
- Fix trace log in the common log format
- Fix timezone parsing in date strings
- Improve the calculation of the etag response header
- ServerName strictly enforced
- Update API stability classifications
- Samples updated
- Documentation updated

### Build Requirements
- To build, requires MakeMe 0.10.0 or later
- To install packages, use Pak 0.12.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.6.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v5.5.1] - 2015-10-28

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Fix OpenSSL using /usr/include for cross builds
- Fix request timeouts for NanoSSL and client requests
- Fix building for ROM support
- Other fixes

### Build Requirements
- To build, requires MakeMe 0.10.0 or later
- To install packages, use Pak 0.12.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.5.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v5.5.0] - 2015-09-23

# Minor Feature Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Add MbedTLS support. This is a non-breaking feature addition to support MbedTLS 2.1.1.
- Fix session cookie handling
- Fix mprSort
- Fix ESP compile mixed modes
- Fix SSL write failure with pipelined requests
- Optimize mprCreateEvent and replace mprCreateEventOutside
- Fix ESP ediSortGrid to handle numeric columns
- Fix various error return paths
- Ignore requests when shutting down
- Ignore disabled monitors

### Build Requirements
- To build, requires MakeMe 0.10.0 or later
- To install packages, use Pak 0.12.0 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.5.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v5.4.7] - 2015-08-26

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Fix for mprCreateOutsideEvent with MPR_EVENT_BLOCK
- Update samples

### Build Requirements
- To build, requires MakeMe 0.9.3 or later
- To install paks, use Pak 0.11.3 or later

### See
- [Includes 5.4.5 Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.7)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v5.4.6] - 2015-08-13

# Minor Patch Release

### Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

## Change Log
- Fix mprCreateOutsideEvent API
- Fix sncontains API

### Build Requirements
- To build, requires MakeMe 0.9.2 or later
- To install paks, use Pak 0.11.3 or later

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.6)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)

## [v5.4.5] - 2015-07-20

# Minor Patch Release

## Recommended Action
- [ ]  Essential Upgrade -- All users strongly advised to upgrade
- [x]  Recommended Upgrade -- Upgrade recommended but not essential
- [ ]  Optional Upgrade -- Upgrade only if convenient

### Change Log
- Fix ESP compiling applications in release mode
- Fix espMail failing to send email
- Fix formatting JSON strings with embedded non-alnum characters
- Move ESP db/migrations up on level to plain **migrations**

#### Build Requirements
- To build, requires MakeMe 0.9.1 or later
- To install paks, use Pak 0.11.2 or later

#### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.5)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v5.4.4] - 2015-06-17

### Minor fix release
- Refactor SSL providers
- Move ESP migrations directory up one level
- Support MakeMe/Pak latest versions

### Build Requirements
- To build, requires MakeMe 0.9.0 or later
- To install paks, use Pak 0.11.1 or later

See:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.4)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v5.4.3] - 2015-06-11

### Minor fix release
- Fix missing API documentation
- Add prototype support for MbedTLS and deprecate the EST SSL stack

### Build Requirements
- To build, requires MakeMe 0.8.10 or later
- To install paks, use Pak 0.11.0 or later

See:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.3)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v5.4.2] - 2015-06-08

### Minor fix release
- Fix configure with openssl
- Fix building samples

### Build Requirements
- To build, requires MakeMe 0.8.10 or later
- To install paks, use Pak 0.11.0 or later

See:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.2)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v5.4.1] - 2015-06-05

Minor feature release.
- Convert to use SSL packages mpr-openssl, mpr-matrixssl, mpr-nanossl, mpr-mbedtls, mpr-est
- Improve configurable makefiles
- Fix PPC cross compiling issue
- Update CA root certificate bundle
- Fix ESP compiling

### Build Requirements
- To build, requires MakeMe 0.8.8 or later
- To install paks, use Pak 0.10.2 or later

See:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v5.4.0] - 2015-04-14

### Appweb 5.4.0 Feature Release.

Appweb 5.4 is a major feature release and the [ESP](https://embedthis.com/esp) web framework is significantly upgraded. Configuration for ESP applications now contained in a dedicated **esp.json** file and the routing of ESP URLs has been simplified and made more uniform. Appweb now includes support for the [Expansive](https://embedthis.com/expansive/) web site generator.

WARNING: This release has breaking changes that will require modifications to existing applications. Please expect some time to adapt your application. Our goal is to synchronize these changes between version 5 and 6 so there will be fewer changes going forward. See below for more details.

### Build Requirements
- To build, requires MakeMe 0.8.6 or later
- To install paks, use Pak 0.10.0 or later
- For Expansive, use Expansive 0.5.0 or later

### Directions

Appweb 5.4 takes a major step toward using JSON configuration files for ESP and for Appweb that are smaller, faster, simpler and more flexible. Appweb 5 still supports the older Apache style configuration, but this will be changed in Appweb 6 to the new JSON alternative that is more flexible.

Appweb 5.4 incorporates full support for the Expansive web environment. [Expansive](https://embedthis.com/expansive/) brings templates, layouts, partials and per-page meta data to any web application. Appweb 5.4 also integrates more closely with the [Pak](https://embedthis.com/pak/) package manager which provides an online [Pak Catalog](https://embedthis.com/catalog/#/) of extension packages for Appweb and ESP.

#### Features
- Move all ESP configuration from appweb.conf and package.json into esp.json. The ESP directives in the appweb.conf file are now reduced to just a simple EspApp directive.
- Support clean URLs for ESP pages without the need for a separate server prefix for server-side Ajax requests.
- Support using Expansive for templating, layouts, partials and web application building. This replaces the ESP-specific layouts and makes the feature available to all HTML, ESP and other web pages. Expansive offers greater flexibility for creating and managing web content as well as a growing library of plugins.
- The PHP and Ejscript handlers have been moved to their own repository at https://github.com/embedthis/appweb-php and https://github.com/embedthis/appweb-ejs. These are now installed by "pak install appweb-php" and "pak install appweb-ejs".

#### Breaking Changes

The ESP web framework in Appweb 5.4 has some breaking changes for ESP MVC applications. These changes affect RESTful routes and the naming of actions and views. These changes are made to support using clean URLs, esp.json, Expansive and Pak for modular ESP packages that can be more easily installed and upgraded.
- Modified all Esp\* appweb.conf directives. EspApp is now simplified and points to an esp.json configuration file. Other Esp directives including EspDb, EspRoute, EspDir, EspCompile, EspLink, EspEnv, EspResource, EspPermResource, EspResourceGroup, EspKeepSource, EspRouteSet and EspUpdate are removed. All have equivalent configuration in esp.json.
- The esp.conf compiler definitions file is converted to esp.json.
- See a full list of ESP changes and migration recommendations at: [ESP 5.4.0](https://github.com/embedthis/esp/releases/tag/untagged-d92251e90d7b91323895)
- Moved the PHP and Ejscript handlers to external packages. See appweb-php and appweb-ejs in the Pak catalog.

References:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.4.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)
- [ESP Documentation](https://embedthis.com/esp/doc/index.html)


## [v5.3.0] - 2015-04-10

### Appweb 5.3.0 Feature Release.
- Enhance authentication management for AuthType.
- Enhance authentication configuration via ESP package.json files.
- Improve log route display with "appweb -s".
- Clarify documentation regarding Basic and Digest authentication (Don't use).
- Add new samples: login-form, login-basic.
- Fix Makefiles for openssl.
- Fix broken links in documentation.
- Fixes from 4.6.6.

References:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.3.0).
- [Download](https://embedthis.com/appweb/download.html).
- [Documentation](https://embedthis.com/appweb/doc/index.html).


## [v5.2.0] - 2014-10-31

### Appweb 5.2.0 Feature Release.
- SSL POODLE fix.
- New documentation layout and design using Expansive for layout.
- Improved Windows nmake and Visual Studio projects.
- Various cleanup, improved tests and documentation updates.
- MakeMe 0.8.4 support with plugins.

References:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A5.2.0).
- [V4.6.5 Issues applied](https://github.com/embedthis/appweb/issues?q=milestone%3A4.6.5).
- [V4.6.4 Issues applied](https://github.com/embedthis/appweb/issues?q=milestone%3A4.6.4).
- [Download](https://embedthis.com/appweb/download.html).
- [Documentation](https://embedthis.com/appweb/doc/index.html).

## v5.1.0 - 2014-08-09

Issues addressed by this release:
- [Change Log](https://github.com/embedthis/appweb/issues?q=milestone%3A5.1.0)

## New Features
- Simplify log and trace output to the console via --trace and --log switches
- Add unit tests using TestMe

## [v4.7.3] - 2015-06-17

Minor patch release.
- Support MakeMe 0.9 and Pak 0.11

### Build Requirements
- To build, requires MakeMe 0.9.0 or later
- To install paks, use Pak 0.11.1 or later

See:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A4.7.3)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)



## [v4.7.2] - 2015-06-05

Minor feature release.
- Fix digest authentication
- Fix lower case mapping of esp platform string
- Fix parsing listen socket addresses.

### Build Requirements
- To build, requires MakeMe 0.8.8 or later
- To install paks, use Pak 0.10.2 or later

See:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A4.7.2)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v4.7.1] - 2015-04-24

Minor feature release.
- Fix use of ESP with new [Pak](https://embedthis.com/pak) v0.10.0 which changed the layout of the Pak ~/.paks cache.
- Added MakeMe and Pak version requirements to main.me and package.json.
- Fix incorrect mime types for CGI [issue 449]

### Build Requirements
- To build, requires MakeMe 0.8.7 or later
- To install paks, use Pak 0.10.1 or later

See:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A4.7.1)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v4.7.0] - 2015-04-14

Minor feature release.
- Ability to create persistent cookies
- Disable web socket data transfer limits

### Build Requirements
- To build, requires MakeMe 0.8.6 or later
- To install paks, use Pak 0.10.0 or later
- For Expansive, use Expansive 0.5.0 or later

See:
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A4.7.0)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)


## [v4.6.6] - 2014-12-03

Minor feature release.
- Fix parsing invalid HTTP Range header.
- Fix returning 401 instead of 403 for unauthorized user.
- Redirect secure to preserve original scheme protocol.

See:
- [Change Log](https://github.com/embedthis/appweb/issues?q=milestone%3A4.6.6).
- [Download](https://embedthis.com/appweb/download.html).
- [Documentation](https://embedthis.com/appweb/doc/index.html).

## [v4.6.5] - 2014-10-31

Minor feature release.
- Fix for invalid CGI programs with delayed responses.
- Fix to be able to select TLS versions.
- Fix for POODLE SSL 3 threat.

See:
- [Change Log](https://github.com/embedthis/appweb/issues?q=milestone%3A4.6.5).
- [Download](https://embedthis.com/appweb/download.html).
- [Documentation](https://embedthis.com/appweb/doc/index.html).


## [v4.6.4] - 2014-09-17

Minor patch release.
- [Change Log](https://github.com/embedthis/appweb/issues?q=milestone%3A4.6.4).
- [Download](https://embedthis.com/appweb/download.html).
- [Documentation](https://embedthis.com/appweb/doc/index.html).


## [v4.6.3] - 2014-08-25

Minor patch release.
- [Change Log](https://github.com/embedthis/appweb/issues?q=milestone%3A4.6.3).
- [Download](https://embedthis.com/appweb/download.html).
- [Documentation](https://embedthis.com/appweb/doc/index.html).

## [v3.4.5] - 2021-06-10

# Minor Patch Release

### Recommended Action

- [ ]  Optional Upgrade -- Upgrade only if convenient
- [ ]  Recommended Upgrade -- Upgrade recommended but not essential
- [x]  Essential Upgrade -- All users strongly advised to upgrade

### Change Log

* Fix socket ioEvent EOF handling

### See
- [Issues](https://github.com/embedthis/appweb/issues?q=milestone%3A3.4.5)
- [Download](https://embedthis.com/appweb/download.html)
- [Documentation](https://embedthis.com/appweb/doc/index.html)
