1.3.0
===========

Features
--------
* [CPP-360] Added the ability to specify custom memory allocators using
  `cass_alloc_set_functions()`
* [CPP-417] Added speculative execution metrics which can be obtained using
  `cass_session_get_speculative_execution_metrics()`
* [CPP-466] Token aware routing replicas are now randomly shuffled by default
  instead of using a random index

Bug Fixes
--------
* [CPP-368] API Functions that take strings now check for NULL and replace it
  with an empty string
* [CPP-437] Fixed batch encoding performance regression
* [CPP-477] Native protocol "support" messages are now properly decoded, but
  still remain unused
* [CPP-487] Fixed a Kerberos regression caused by providing an empty principal
* [CPP-501] Schema metadata is corrupted when views and indexes exist for a table

Other
--------
* [CPP-136] Decoding now verifies buffer sizes when processing server responses
* [CPP-432] Removed dense table 'empty' columns from metadata
* [CPP-440] Improved encoding performance by preallocating buffers
* [CPP-444] Fixed const correctness of serialization functions
* [CPP-476] Replaced `strlen()` with `sizeof()` for static strings
* [CPP-478] Renamed the class `SpeculativeExecution` to `RequestExection` to
  reduce confusion when it appears in driver logs
* [CPP-480] Added more detailed documentation to help with using Kerberos
  authentication

Merged from OSS 2.7.0
--------
* [CPP-464] Adding a node breaks token map reconstruction
* Fixed an issue where UUID parsing could read past the supplied string
* Fixed various typos in `cassandra.h`
* Fixed missing header for intrinsics when building with MSVC15

1.2.0
===========

Features
--------
* Added support for DSE proxy authentication
* Added support for DSE `DateRangeType`
* Added support for adding DSE geospatial types as elements of collections
* Added support for `DurationType`
* Added support for OpenSSL 1.1
* Added support for protocol DSEv1 and V5 (beta)

1.1.0
===========

Features
--------
* Expose wkt parsing logic for public use:
  * `dse_point_from_wkt()`, `dse_point_from_wkt_n()`
  * `dse_line_string_iterator_reset_with_wkt()`,
    `dse_line_string_iterator_reset_with_wkt_n()`
  * `dse_polygon_iterator_reset_with_wkt()`,
    `dse_polygon_iterator_reset_with_wkt_n()`

Other
---------
* [CPP-416] LineString::to_wkt and Polygon::to_wkt now return "LINESTRING
  EMPTY" and "POLYGON EMPTY", respectively, for empty
  objects.
* [CPP-416] WKT parsing for "LINESTRING EMPTY" and "POLYGON EMPTY" now
* succeeds.

1.0.0
===========
October 20, 2016

Features
--------
* Added a host targeting policy for graph analytics queries. Graph queries with
  the source "a" will attempt to be routed directly to the analytics master
  node.
* Added geometric types support for graph queries.

1.0.0-rc1
===========
June 24, 2016

Features
--------
* Added a graph option to set graph query request timeout (default: no timeout)
* Added graph options to set the consistency level of graph query requests
* Added the ability to set the timestamp on graph statements

Other
--------
* Changed the default graph source from "default" to "g"
