# StyleWriter v2

Here's a quick guide to StyleWriter!

StyleWriter is tailored to choropleth and scaled-point maps, although it can
help with the rendering of any single-datasource TileLive-rendered maps.
It collaborates with a verson of [TileLite][tilelite] called 
[TileLive][tlv] that is available on GitHub. It requires the 
[OpenLayers module][olmod], and also works with the 
[openlayers_plus][olp] module to provide legends and allow quick swapping if 
you have both a point and a choropleth map displaying the same information.

## Requirements

* A new version of jQuery (either by jquery_update or hacking)
* OpenLayers module
* A working TileLive server with Mapnik 2

## Included Functionality

* A Display Plugin and Style Plugin to views that create mapfiles 
  and legends for choropleth layers on maps
* Utility functions to blend colors into each other to form the 
  basis for choropleth maps
* A layer type that implements the requirements of the TileLite 
  branch used
* Behaviors that enable interaction with grid-based (polygon) and
  GeoJSON/metawriter (point) maps

## Why This Is Necessary

StyleWriter and its branch of TileLite don't follow the typical convention of 
map servers and clients. Instead of configuring layers on the tile server, 
the datasource and style applied to it is defined on a per-tile basis by 
providing URLs of the resources for the tile server to pull. This means that 
the combo can handle situations in which the data does not and cannot reside 
on the tile server itself, and in which the styles applied to the data are 
not limited in any sense.

## Using StyleWriter with Views

Given the complex nature of what StyleWriter's views integration does, it 
requires the use of a display plugin and style plugin in conjunction. The 
style plugin is written to be close to the OpenLayers Data style plugin 
in configuration, despite the fact that the underlying structure is much 
different.

1. Add a new *Mapfile* display to a View
2. Set the style to *Mapfile*
3. Configure the style plugin

    Minimum & maximum colors: these are the colors that will be assigned to 
      the minimum and maximum (in numeric terms) values of the dataset. 
      The colors are blended together into shades numbering the value of 
      'Gradations' by converting the colors to HSL (Hue, Saturation, Luminosity)
      and linearly stretching between each component

    Join field name: This is the name of the join field _in the datasource_ - 
      thus, if the Shapefile refers to countries by a column named 'FIPS', 
      then this field should contain the text FIPS. This is equivalent to 
      a foriegn key in other systems.

    Join field: the value of the join field for this view. The options are 
      taken from the fields that are available to the view.

    Value: the value of the data. This can be an integer or float field. Note 
      that the join and value fields are pushed into mapfiles without 
      being rendered by the views stack - so that they are most likely 
      'pure' values without formatting

4. If the view is properly configured, it will be available when creating 
   a new StyleWriter layer.

## Using StyleWriter with OpenLayers

StyleWriter provides a layer type for OpenLayers. Although technically, one 
could use the XYZ layer type for this purpose, but the included layer type 
includes some improvements and customizations.

* Automatically passes arguments from the OpenLayers Map display URL to the 
  underlying mapfile URL
* URL-safe base64 encodes the mapfile and datafile arguments to the URL
* Allows XYZ layers to have restricted minimum zoom levels
* Queries the underlying mapfile view to provide a legend in the Javascript
  layer array

## Using StyleWriter with OpenLayers Plus

The legend that the StyleWriter layer type puts into its array (under 
options->legend) is not, by default, visible on the map. With the help of 
the openlayers_plus module's legend control, it can be displayed on the map.

The OpenLayers Plus module also provides a Block Toggle control which provides 
a toggle between two different layers - allowing users to switch between, 
for instance, a point and choropleth representation of the same data.

## Creating Mapfiles

The StyleWriter module contains a unique Views display / style combination 
that, primarily writes [Mapnik][m] map files. Given a data setup that provides 
one field that is made up of integers or float values, StyleWriter is able to 
calculate equal-spaced groups of values and assign them colors. The styles 
that StyleWriter writes are _not_ rule-based - they are not dependent on the 
*data* argument actually containing the data that they're styling on.

For instance, if your dataset is countries colored by income per capita, the 
Shapefile need not contain the actual statistics - it only needs to contain 
the 'join field' which you also specify in the views style plugin. When the 
style is written, it contains rules in the form

    country[name="America"] { color: red }

by comparison, if the alternative model was used - the 'semantic' model, so to 
speak, then the rule would look like

    country[income_per_capita > 300] { color: red }

The difference between the two approaches is that the StyleWriter considers 
data files to contain _shape data_, not _value data_, but for the value data 
to be managed by Drupal and updated more often. Adding more columns, therefore, 
is trivial in this implementation, whereas with the semantic model it would 
require extensive cooperation of Drupal and the datasource.

## Interaction

The StyleWriter module provides tools for two types of interactivity: broadly
for polygon-based maps and point-based maps. Both of these techniques require
TileLive to be run with its internal `tile_cache` setting on, since they 
take advantage of Mapnik's metawriter support and aggressively cache their 
results.

In order to use point-based interaction, enable the Point Interaction behavior.
In order to use polygon-based interaction, enable the 
Polygon Interaction behavior.

The Polygon Interaction behavior writes a Javascript 'mapping table' to the 
page source, so it can increase the size of pages if thousands of polygons 
are present. The Point Interaction behavior loads full point data dynamically, 
so it is better suited for thousands of features.

# GeoJSON

Currently this module is tuned to with GeoJSON output from OpenLayers, which 
is [available on Drupal.org as a patch](http://drupal.org/node/889190) currently.

With this setup, it is possible to create scaled points based on data in 
Drupal - outputting style data as Cascadenik style rules.


## Data

The StyleWriter module supports GeoJSON and shapefiles as datasources. If 
data is not dynamic or contains complex datatype like polygons, it is best 
to use shapefiles rather than GeoJSON.

## Drawbacks

The combination of datasource and stylesheet is not strictly defined, but it 
is strict. Unlike CSS, if the map file created has a field that doesn't exist 
in the data, the map cannot be rendered. Thus it is necessary to be aware 
of the specific data that the data file contains when creating mapfiles, either 
freeform or with this module.

## Troubleshooting

* **Map interactivity doesn't work**: jQuery must be updated and the TileLive server
  must be running Mapnik 2.

### Changes from v1

* The location of the data file is now owned by the view style plugin, rather
  than the layer.
* The StyleWriter module now contains interactivity plugins

[olmod]: http://drupal.org/project/openlayers
[tilelite]: http://bitbucket.org/tmcw/tilelite
[olp]: http://github.com/developmentseed/openlayers_plus
[c]: http://code.google.com/p/mapnik-utils/wiki/Cascadenik
[m]: http://www.mapnik.org/
[d]: http://drupal.org/project/drush
[ts]: http://tilestache.org/
[tlv]: http://github.com/tmcw/TileLiteLive
