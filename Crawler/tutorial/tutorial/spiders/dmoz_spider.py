#!/usr/bin/python
#-*- coding:utf-8 -*-
import scrapy

from tutorial.items import DmozItem

class DemozSpider(scrapy.Spider):
    name = "dmoz"
    allowed_domains = ["dmoz.org"]
    start_urls = [
        "http://write.blog.csdn.net/postlist"    ]
    
    def parse(self, response):
        for sel in response.xpath('//ul/li'):
            item = DmozItem()
            item['title'] = self.xpath('a/text()').extract()
            item['link'] = sel.xpath('a/@herf').extract()
            item['desc'] = sel.xpath('text()').extract()
            yield item
     #       print title, link, desc
     #   filename = response.url.split("/")[-2]
     #   with open(filename, "wb", encoding='utf-8') as f:
     #       f.write(response.body)
