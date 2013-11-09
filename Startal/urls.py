from django.conf.urls.defaults import *
from django.conf import settings

from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    (r'^$',      'Startal.Chrome.views.index'),
    (r'^admin/', include(admin.site.urls)),
    (r'^site_media/(?P<path>.*)$', 'django.views.static.serve',
       {'document_root': settings.STATIC_DOC_ROOT}),
    (r'^kmz_files/(?P<path>.*)$', 'django.views.static.serve',
       {'document_root': settings.KMZ_DOC_ROOT}),
#Elevation
    (r'^elevation_tab/$', 'Startal.Chrome.elevation_views.elevation_tab'),
    (r'^get_elevation_params/(?P<elevation_type>.+)/$', 'Startal.Chrome.elevation_views.get_elevation_params'),
    (r'^add_elevation_data/(?P<name>.+);;;;(?P<elevation_type>.+);;;;(?P<elevation_flags>.*)/', 'Startal.Chrome.elevation_views.add_elevation_data'),
    (r'^remove_elevation/(?P<elevation_id>\d+)/$', 'Startal.Chrome.elevation_views.remove_elevation'),
#Sar Data
    (r'^sar_data_tab/$', 'Startal.Chrome.sar_data_views.sar_data_tab'),
    (r'^add_sar_data/(?P<name>.+);;;;(?P<antenna_type>.+);;;;(?P<antenna_flags>.+);;;;(?P<elevation_id>\d+)/', 'Startal.Chrome.sar_data_views.add_sar_data'),
    (r'^get_antenna_params/(?P<antenna_type>.+)/(?P<elevation_id>\d+)/', 'Startal.Chrome.sar_data_views.get_antenna_params'),
    (r'^remove_sar_data/(?P<sar_data_id>\d+)/$', 'Startal.Chrome.sar_data_views.remove_sar_data'),
#Rois
    (r'^roi_tab/$', 'Startal.Chrome.roi_views.roi_tab'),
    (r'^rois/(?P<roi_id>\d+)/',
      'Startal.Chrome.roi_views.roi_to_kml'),
    (r'^add_roi/(?P<ulLat>.+)/(?P<ulLon>.+)/(?P<urLat>.+)/(?P<urLon>.+)' + \
     r'/(?P<lrLat>.+)/(?P<lrLon>.+)/(?P<llLat>.+)/(?P<llLon>.+)/$',
      'Startal.Chrome.roi_views.add_roi'),
    (r'^remove_roi/(?P<roi_id>\d+)/$', 'Startal.Chrome.roi_views.remove_roi'),
#Tasks
    (r'^add_task/(?P<sar_data_id>\d+)/(?P<roi_id>\d+)/(?P<elevation_id>\d+);;;;(?P<algorithm_type>.+);;;;(?P<algorithm_flags>.+)/$', 'Startal.Chrome.views.add_task'),
    (r'^task_tab/$', 'Startal.Chrome.views.task_tab'),
    (r'^get_algorithm_params/(?P<antenna_id>\d+)/(?P<elevation_id>\d+)/(?P<algorithm_type>.+)/$', 'Startal.Chrome.views.get_algorithm_params'),
    (r'^remove_task/(?P<taskId>\d+)/$', 'Startal.Chrome.views.remove_task'),
)
