# Using StyleWriter

## What a StyleWriter map consists of:

* Mapfile display: a views display that provides MML to Cascadenik to Mapnik to TileLive
* A datasource: either a shapefile at an arbitrary URI or a GeoJSON feed provided by the GeoJSON patch to OpenLayers and the OpenLayers GeoJSON style plugin (via feeds) which takes advantage of it.
* StyleWriter layer: an OpenLayers layer that points to the mapfile display and TileLive server, telling the two to talk
* [optional] StyleWriter Polygon Interaction / StyleWriter Point Interaction: configuration-free interaction behaviors to talk to TileLive's vector interaction data
* OpenLayers Preset: a mishmash of the layer and behaviors.

## How a stylewriter map works.

* Mapfile displays have urls: http://drupal.biz/mapfile
* TileLive servers have urls: http://tilelive.biz/
* Shapefiles have urls: http://s3.biz/shapefile.shp
* Mapfiles point to shapefiles
* StyleWriter layers point to mapfiles and tilelive servers, putting the URL of the former in requests to the latter
* TileLive downloads the shapefile from wherever and the mapfile from Drupal and caches them
* TileLive serves up tiles of images, point data, and grid data

## Data setup

StyleWriter's ability to generate choropleth maps - by dynamically writing mapfiles - is dependent on the correspondence of data in Drupal with data in the provided shapefile datasources. The most important part is the existence of a valid **join field** - a column in a shapefile and a field in Drupal. The naming of this field in Drupal is flexible - in fact, in the Mapfile configuration, you can simply type the name of the field in the shapefile and Drupal will use that name in the join. The Mapfile configuration can also represent the field in Drupal as an integer or a string to work with the shapefile's configuration.

## StyleWriter configuration

The StyleWriter Mapfile style plugin is a sprawling alley of configuration.

* Scaled points work with GeoJSON datasources
* Choropleth work with Shapefile datasources
* There's an 'extra' field for any other styles necessary
* MetaWriters are required for point interaction (not required for grid interaction)

## Legends

To enable legends, one must use the openlayers_plus module and enable legends on a given map. Choropleth legends can be entirely automated but point legends are not: generating circles is not HTML-friendly in the same way as generating legend output.

## Choropleth Mapfile display

* Create a Mapfile display

![Mapfile display](http://img.skitch.com/20100921-rb33r1k6ju3tmcsbym8astfptw.png)



## Gotchas / TODO:

* Multiple StyleWriter layers cannot be interacted with at the same time: the one on top rules.
* **The OpenLayers XYZ layer type on which StyleWriter layers are based is very poorly implemented** and therefore one must actually change the zoom level at which these layers start within the layer type currently.
* **The TileLive server requests the Mapfile from Drupal**: Thus, if Drupal is behind a password or obscured some other way from the TileLive server, maps will be inaccessible. The same goes for datasources, both shapefiles and GeoJSON: if TileLive cannot get them, it cannot get them.
* Point legends are not entirely automated because they need images of points to operate correctly, and deriving these images from mapfiles would be kind of awful.
