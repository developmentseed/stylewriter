# StyleWriter v2

**Currently the 'master' branch of this repository is used for developing version 2 of StyleWriter, compatible with version 2 of TileLiteLive. Use the 'v1' branch for compatibility with version 1.**

Here's a quick guide to StyleWriter!

StyleWriter is tailored to choropleth maps. It collaborates with a verson of 
[TileLite][tilelite] called [TileLive][tlv] that is available on GitHub.
It requires the [OpenLayers module][olmod], and also works with the 
[openlayers_plus][olp] module to provide legends and allow quick swapping if 
you have both a point and a choropleth map displaying the same information.

## Included Functionality

* It provides a Display Plugin and Style Plugin to views that create mapfiles 
  and legends for choropleth layers on maps
* It provides utility functions to blend colors into each other to form the 
  basis for choropleth maps
* It provides a layer type that implements the requirements of the TileLite 
  branch used
* It provides a Drush command to run a Python script, tilelite-seed.py, that 
  seeds - populates the caches - of mapfile / data combinations on a 
  given server
* It provides an administration interface that allows you to view the 
  currently-cached data / mapfile combinations and clear those caches on command

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

## Using StyleWriter with Drush

StyleWriter includes a [Drush][d] command called 'stylewriter-cache-all.' 
Running

    $ drush stylewriter-cache-all

Causes StyleWriter to prepopulate _tile_ caches by calling a Python script 
called `tileseed.py`, which must be provided by the user. This script 
is included in the branch of TileLite and is dependent on [TileStache][ts]. 
However, if two hooks aren't implemented in any other modules, no caches will 
be populated - since maps are dependent on a data and mapfile URL argument, 
without either, maps cannot be rendered or cached. The two hooks called are 

    /**
     * must return an array of fully-formed URLs pointing to 
     * mapfiles
     */
    hook_stylewriter_mapfile_possible_facets()

    /**
     * must return an array of fully-formed URLs pointing to 
     * data files
     */
    hook_stylewriter_data_possible_facets()

A simple implementation of the data hook could be

    /**
     * Implementation of hook_stylewriter_data_possible_facets()
     */
    function wbboxes_stylewriter_data_possible_facets() {
      return array(
        'http://demo.com/shapefile_zipped.zip'
      );
    }

In order to implement the mapfile hook, one must generate URLs - often views 
URLs with specific URL-arguments.

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
that, primarily writes [Mapnik][m] XML files. Given a data setup that provides 
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

Although [Cascadenik][c] support is planned, the current implementation isn't 
viable for this project.

## Assumptions and customizations

There are defaults built into this module - the default rule template - 
contained in `views/stylewriter-rule.tpl.php` sets the `fill-opacity` of polygons 
to 0.5 and doesn't add a stroke of any kind to the rendered polygons. Also,
it is a current assumption of the module that the data file contains shapes, 
rather than points or lines, as the default rule doesn't contain anything 
but PolygonSymbolizer. Also, this module doesn't aim to create maps in projects 
other than EPSG:900913, the Spherical Mercator Google-style projection, or to 
create maps with backgrounds other than 'transparent'.

One can currently override these assumptions by including a custom 
`stylewriter-rule.tpl.php` in a theme, but this is a global override - there is 
no real, current functionality for having different 'rule styles' per different 
kinds of maps. One can also override the default document style (the fact that 
maps have a transparent background) by adding a `stylewriter-document.tpl.php` 
file to a theme.

## Data

Originally the TileLite branch included support for both GeoJSON and Shapefiles.
Currently it only supports Shapefiles, because the performance of abstracted 
vector data sources in Mapnik - datasources supported by OGR - is not 
comparable to the native Shapefile driver. In the future, support for more
datasources will be included.

## Drawbacks

The combination of datasource and stylesheet is not strictly defined, but it 
is strict. Unlike CSS, if the map file created has a field that doesn't exist 
in the data, the map cannot be rendered. Thus it is necessary to be aware 
of the specific data that the data file contains when creating mapfiles, either 
freeform or with this module.

[olmod]: http://drupal.org/project/openlayers
[tilelite]: http://bitbucket.org/tmcw/tilelite
[olp]: http://github.com/developmentseed/openlayers_plus
[c]: http://code.google.com/p/mapnik-utils/wiki/Cascadenik
[m]: http://www.mapnik.org/
[d]: http://drupal.org/project/drush
[ts]: http://tilestache.org/
[tlv]: http://github.com/tmcw/TileLiteLive
