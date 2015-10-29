from django.conf.urls import patterns, include, url
from django.contrib import admin

from django.http import HttpResponse

def index(request, batch):
    try:
        with open('/highway/strans_621/GArun_' + batch + "/evolution.txt") as f:
            content = f.read()
            return HttpResponse(content, content_type="text/plain")
    except:
        return HttpResponse("ERROR")

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'live_results.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

                       url(r'^(?P<batch>[^/]+)/$', index),
)
