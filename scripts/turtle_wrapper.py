from turtle import *
import tkinter as tk
import sys

class TurtleGraphWrapper:
    
    def __init__(self, base_path_start,  base_path_end, screen_x, screen_y):
        coords1 = self.processFile(base_path_start)
        coords2 = self.processFile(base_path_end)

        self.minLat = 90
        self.minLon = 180
        self.maxLat = 0
        self.maxLon = 0

        lats = coords1[0]
        lons = coords1[1]

        for lat in lats:
            if lat > self.maxLat:
                self.maxLat = lat
            if lat < self.minLat:
                self.minLat = lat

        for lon in lons:
            if lon > self.maxLon:
                self.maxLon = lon
            if lon < self.minLon:
                self.minLon = lon

        lats = coords2[0]
        lons = coords2[1]

        for lat in lats:
            if lat > self.maxLat:
                self.maxLat = lat
            if lat < self.minLat:
                self.maxLon = lon

        for lon in lons:
            if lon > self.maxLon:
                self.maxLon = lon
            if lon < self.minLon:
                self.maxLon = lon

        self.conv = LatLonConverter(self.minLat, self.maxLat, self.minLon, self.maxLon, screen_x, screen_y)
        self.screen = Screen()
        self.screen.title("Routing Visualization")
        self.screen.tracer(0)
        self.screen.setup(2 * (self.conv.maxX + self.conv.xOffset), 2 * (self.conv.maxY + self.conv.yOffset))
        self.turtle = Turtle()
        self.turtle.speed(0)
        self.turtle.hideturtle()
        self.turtle.width(1)
        self.drawEdges(coords1[0], coords1[1], coords2[0], coords2[1], '#000000')

    def drawPathFromFile(self, start_path, end_path, color):
        coords1 = self.processFile(start_path)
        coords2 = self.processFile(end_path)
        self.turtle.pensize(2)
        self.drawEdges(coords1[0], coords1[1], coords2[0], coords2[1], color)
        self.turtle.pensize(1)

        sourceLat = coords1[0][0]
        sourceLon = coords1[1][0]
        targetLat = coords2[0][len(coords2[0]) - 1]
        targetLon = coords2[1][len(coords2[1]) - 1]

        sourceCoords = self.conv.getPixels(sourceLat, sourceLon)
        targetCoords = self.conv.getPixels(targetLat, targetLon)

        self.turtle.penup()
        self.turtle.goto(sourceCoords[0], sourceCoords[1])
        self.turtle.dot(8, "#32a852")

        self.turtle.penup()
        self.turtle.goto(targetCoords[0], targetCoords[1])
        self.turtle.dot(8, "#a83232")
        
        self.turtle.penup()

    def processFile(self, path):
        file = open(path)
        lats = []
        lons = []
        for line in file:
            strs = line.split(",")
            lats.append(float(strs[0]))
            lons.append(float(strs[1]))
        return (lats, lons)


    def drawEdgesFromFile(self, starts_path, ends_path, color):
        startCoords = self.processFile(starts_path)
        endCoords = self.processFile(ends_path)
        self.drawEdges(startCoords[0], startCoords[1], endCoords[0], endCoords[1], color)
    

    def drawEdges(self, start_lats, start_lons, end_lats, end_lons, color):
        self.turtle.pencolor(color)
        for i in range(len(start_lats)):
            rStart = self.conv.getPixels(start_lats[i], start_lons[i])
            rEnd = self.conv.getPixels(end_lats[i], end_lons[i])

            self.turtle.penup()
            self.turtle.goto(rStart[0], rStart[1])
            self.turtle.pendown()
            self.turtle.goto(rEnd[0], rEnd[1])

            if i % 5000 == 0:
                self.screen.update()

        self.turtle.penup()

    def drawVerticesWithNumbers(self, vertex_path):
        xs = []
        ys = []
        indices = []

        fVertex = open(vertex_path)

        for line in fVertex:
            parts = line.split(",")
            r = self.conv.getPixelsFromStrings(parts[1], parts[2])
            xs.append(r[0])
            ys.append(r[1])
            indices.append(parts[0])


        for i in range(0, len(xs)):
            self.turtle.penup()
            self.turtle.goto(xs[i], ys[i])
            self.turtle.dot(12, "#7fafb3")
            self.turtle.color("#cf6dcd")
            self.turtle.write(indices[i], font=("Arial", 24, "bold"))


    def done(self):
        self.screen.update()
        self.screen.getcanvas().postscript(file="./output/temp/out.eps")


class LatLonConverter:
    def __init__(self, minLat, maxLat, minLon, maxLon, screen_x, screen_y):
        self.minLat = minLat
        self.minLon = minLon
        self.latSpan = maxLat - minLat
        self.lonSpan = maxLon - minLon
        self.screen_x = screen_x
        self.screen_y = screen_y
        self.screen_x_2 = screen_x / 2
        self.screen_y_2 = screen_y / 2

        self.xOffset = 0
        self.yOffset = 0

        pxPerLon = screen_x / self.lonSpan
        pxPerLat = screen_y / self.latSpan

        if (pxPerLon < pxPerLat):
            self.span = self.lonSpan
        else:
            self.span = self.latSpan

        rMax = self.getPixels(maxLat, maxLon)
        
        self.maxX = rMax[0]
        self.maxY = rMax[1]

        self.xOffset = (screen_x / 2 - self.maxX) / 2
        self.yOffset = (screen_y / 2 - self.maxY) / 2

    def getPixels(self, lat, lon):
        latDiff = lat - self.minLat
        lonDiff = lon - self.minLon

        latFac = latDiff / self.latSpan
        lonFac = lonDiff / self.lonSpan

        x = round(lonFac * self.screen_x - self.screen_x_2 + self.xOffset)
        y = round(latFac * self.screen_y - self.screen_y_2 + self.yOffset)

        return (x, y)

    def getPixelsFromStrings(self, latStr, lonStr):
        lat = float(latStr)
        lon = float(lonStr)

        return self.getPixels(lat, lon)
