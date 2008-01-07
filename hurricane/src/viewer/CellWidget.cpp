#include <math.h>

#include <QPaintEvent>
#include <QPainter>
#include <QPen>

#include "DataBase.h"
#include "Technology.h"
#include "BasicLayer.h"
#include "Slice.h"
#include "Box.h"
#include "Segment.h"
using namespace H;

#include "CellWidget.h"

namespace {

Technology* getTechnology() {
    DataBase* database = GetDataBase();
    if (database) {
        return database->GetTechnology();
    }
    return NULL;
}

static QColor  backgroundColor    = QColor(  50,  50,  50 );
static QColor  foregroundColor    = QColor( 255, 255, 255 );
static QColor  rubberColor        = QColor( 192,   0, 192 );
static QColor  phantomColor       = QColor( 139, 134, 130 );
static QColor  boundaryColor      = QColor( 208*255, 199*255, 192*255 );
static QColor  markerColor        = QColor(  80, 250,  80 );
static QColor  selectionDrawColor = QColor( 255, 255, 255 );
static QColor  selectionFillColor = QColor( 255, 255, 255 );
static QColor  gridColor          = QColor( 255, 255, 255 );
static QColor  spotColor          = QColor( 255, 255, 255 );
static QColor  ghostColor         = QColor( 255, 255, 255 );

static QPen boundariesPen       = QPen(boundaryColor);
static QBrush boundariesBrush   = QBrush(boundaryColor);
static QPen phantomsPen         = QPen(phantomColor);
static QBrush phantomsBrush     = QBrush(phantomColor);

}

CellWidget::CellWidget(Cell* c, QWidget* parent)
    : QWidget(parent),
    cell(c),
    invalidRegion(),
    clipBox(),
    clipX(),
    clipY(),
    painter(NULL),
    backgroundColor(20, 20, 20),
    foregroundColor(255, 255, 255),
    scale(1),
    screenDx(0),
    screenDy(0), 
    brushDx(0),
    brushDy(0),
    basicLayersBrush(),
    basicLayersPen() {
    for_each_basic_layer(basiclayer, getTechnology()->GetBasicLayers()) {
        basicLayersBrush[basiclayer] = 
                QBrush(QColor(basiclayer->GetRedValue(), basiclayer->GetGreenValue(), basiclayer->GetBlueValue()));
        basicLayersPen[basiclayer] = 
                QPen(QColor(basiclayer->GetRedValue(), basiclayer->GetGreenValue(), basiclayer->GetBlueValue()));
        end_for;
    }


}

void CellWidget::paintEvent(QPaintEvent* event) {
    invalidate(event->rect());
    redraw();
}

void CellWidget::invalidate() {
    invalidRegion = QRegion(rect());
}


void CellWidget::invalidate(const QRect& screenRect) {
    QRect visibleScreenRect = screenRect.intersect(rect());

    if (!visibleScreenRect.isEmpty()) {
        invalidRegion = invalidRegion.unite(QRegion(visibleScreenRect));
    }
}

void CellWidget::redraw() {
    if (!invalidRegion.isEmpty()) {
        QRect invalidRect = invalidRegion.boundingRect();

        H::Box area = getBox(invalidRect).inflate(getSize(1));

	setClipBox(area);

        QPaintDevice* device = this;

	QPainter newPainter(device);
	QPainter* oldPainter = painter;
	painter = &newPainter;

        double brightness = 1.0;

        //painter->save();
        //setBrush(phantomsBrush, brightness);
        //drawPhantoms(cell, area, Transformation());
        //painter->restore();

        painter->save();
        setPen(boundariesPen, brightness);
        setBrush(boundariesBrush, brightness);
        drawBoundaries(cell, area, Transformation());
        painter->restore();

        for_each_basic_layer(basiclayer, getTechnology()->GetBasicLayers()) {
            painter->save();
            map<BasicLayer*, QBrush>::const_iterator bmit = basicLayersBrush.find(basiclayer);
            if (bmit != basicLayersBrush.end()) {
                setBrush(bmit->second, brightness);
            }
            map<BasicLayer*, QPen>::const_iterator pmit = basicLayersPen.find(basiclayer);
            if (pmit != basicLayersPen.end()) {
                setPen(pmit->second, brightness);
            }
            drawContent(cell, basiclayer, area, Transformation());
            painter->restore();
            end_for;
        }

        invalidRegion = QRegion();
    }
}

void CellWidget::drawContent(const Cell* cell, const BasicLayer* basicLayer, const H::Box& updateArea, const Transformation& transformation) const {
    for_each_instance(instance, cell->GetInstancesUnder(updateArea)) {
        drawContent(instance, basicLayer, updateArea, transformation);
        end_for;
    }

    for_each_slice(slice, cell->GetSlices()) {
        drawSlice(slice, basicLayer, updateArea, transformation);
        end_for;
    }
}


void CellWidget::drawContent(const Instance* instance, const BasicLayer* basicLayer, const H::Box& updateArea, const Transformation& transformation) const {
    Box masterArea = updateArea;
    Transformation masterTransformation = instance->GetTransformation();
    instance->GetTransformation().getInvert().applyOn(masterArea);
    transformation.applyOn(masterTransformation);
    drawContent(instance->GetMasterCell(), basicLayer, masterArea, masterTransformation);
}


void CellWidget::drawSlice(const Slice* slice, const BasicLayer* basicLayer, const H::Box& updateArea, const Transformation& transformation) const {
    if (slice->GetLayer()->Contains(basicLayer)) {
        if (slice->GetBoundingBox().intersect(updateArea)) {
            //if ((basicLayer == _layer->_GetSymbolicBasicLayer()) || (3 < view->GetScale())) 
            for_each_go(go, slice->GetGosUnder(updateArea)) {
                drawGo(go, basicLayer, updateArea, transformation);
                end_for;
            }
        }
    }
}

void CellWidget::drawGo(const Go* go, const BasicLayer* basicLayer, const Box& updateArea, const Transformation& transformation) const {
    const Segment* segment = dynamic_cast<const Segment*>(go);
    if (segment) {
        drawSegment(segment, basicLayer, updateArea, transformation);
        return;
    }
    const Contact* contact = dynamic_cast<const Contact*>(go);
    if (contact) {
        drawContact(contact, basicLayer, updateArea, transformation);
    }
}

void CellWidget::drawSegment(const Segment* segment, const BasicLayer* basicLayer, const Box& updateArea, const Transformation& transformation) const {
    drawRectangle(transformation.getBox(segment->GetBoundingBox(basicLayer)));
}

void CellWidget::drawContact(const Contact* contact, const BasicLayer* basicLayer, const Box& updateArea, const Transformation& transformation) const {
    drawRectangle(transformation.getBox(contact->GetBoundingBox(basicLayer)));
}

void CellWidget::drawPhantoms(const Cell* cell, const H::Box& updateArea, const Transformation& transformation) const {
    for_each_instance(instance, cell->GetInstancesUnder(updateArea)) {
        drawPhantoms(instance, updateArea, transformation);
        end_for;
    }
}

void CellWidget::drawPhantoms(const Instance* instance, const H::Box& updateArea, const Transformation& transformation) const {
    H::Box masterArea = updateArea;
    Transformation masterTransformation = instance->GetTransformation();
    instance->GetTransformation().getInvert().applyOn(masterArea);
    transformation.applyOn(masterTransformation);
    drawPhantoms(instance->GetMasterCell(), masterArea, masterTransformation);
}

void CellWidget::drawBoundaries(const Cell* cell, const H::Box& updateArea, const Transformation& transformation) const {
    drawRectangle(transformation.getBox(cell->GetAbutmentBox()));
    for_each_instance(instance, cell->GetInstances()) {
        drawBoundaries(instance, updateArea, transformation);
        end_for;
    }
}

void CellWidget::drawBoundaries(const Instance* instance, const H::Box& updateArea, const Transformation& transformation) const {
    H::Box masterArea = updateArea;
    Transformation masterTransformation = instance->GetTransformation();
    instance->GetTransformation().getInvert().applyOn(masterArea);
    transformation.applyOn(masterTransformation);
    drawBoundaries(instance->GetMasterCell(), masterArea, masterTransformation);
}

void CellWidget::drawRectangle(const H::Box& box) const {
    if (painter) {
        H::Box ibox = box.getIntersection(clipBox);
        if (!ibox.isEmpty()) {
            int x = getScreenX(ibox.getXMin());
            int y = getScreenY(ibox.getYMax());
            int w = getScreenX(ibox.getXMax()) - x + 1;
            int h = getScreenY(ibox.getYMin()) - y + 1;

            painter->drawRect(x, y, w, h);
        }
    }
}

Unit CellWidget::getX(int screenX) const {
    return GetUnit((screenX - screenDx) / scale);
}

Unit CellWidget::getY(int screenY) const {
    return GetUnit(((height() - screenY) - screenDy) / scale);
}

Unit CellWidget::getSize(int screenSize) const {
    return GetUnit(screenSize / scale);
}

H::Box CellWidget::getBox(const QRect& screenRect) const {
    if (screenRect.isEmpty()) {
        return H::Box();
    }

    return H::Box(getX(screenRect.left()),
            getY(screenRect.bottom()),
            getX(screenRect.right()),
            getY(screenRect.top()));
}

int CellWidget::getScreenX(const Unit& x) const {
    return screenDx + (int)rint(GetValue(x) * scale);
}

int CellWidget::getScreenY(const Unit& y) const {
    return height() - (int)rint(screenDy + (GetValue(y) * scale));
}


void CellWidget::setClipBox(const Box& area) {
    clipBox = Box(area).inflate(getSize(2));

    clipX[0] = GetValue(clipBox.getXMin());
    clipX[1] = GetValue(clipBox.getXMax());
    clipX[2] = GetValue(clipBox.getXMin());
    clipX[3] = GetValue(clipBox.getXMax());
    clipY[0] = GetValue(clipBox.getYMin());
    clipY[1] = GetValue(clipBox.getYMin());
    clipY[2] = GetValue(clipBox.getYMax());
    clipY[3] = GetValue(clipBox.getYMax());
}


void CellWidget::setFont(const QFont& font) {
    if (painter) {
	painter->setFont(font);
    }
}

void CellWidget::setPen(const QPen& pen, double brightness) {
    if (!((0.1 <= brightness) && (brightness <= 1.0))) {
	throw Error(__FILE__, __LINE__);
    }

    if (painter) {
	if (pen == Qt::NoPen) {
	    painter->setPen(pen);
	}
	else {
	    QPen correctedPen = pen;

	    if (brightness < 1) {
		QColor bgColor = getBackgroundColor();
		int r = bgColor.red();
		int g = bgColor.green();
		int b = bgColor.blue();

		QColor color = pen.color();
		r = r + (int)((color.red() - r) * brightness);
		g = g + (int)((color.green() - g) * brightness);
		b = b + (int)((color.blue() - b) * brightness);

		correctedPen = QPen(QColor(r, g, b));
	    }

	    painter->setPen(correctedPen);
	}
    }
}

void CellWidget::setBrush(const QBrush& brush, double brightness) {
    if (!((0.1 <= brightness) && (brightness <= 1.0))) {
	throw Error(__FILE__, __LINE__);
    }

    if (painter) {
	if (brush == Qt::NoBrush) {
	    painter->setBrush(brush);
	}
	else {
	    QBrush correctedBrush = brush;

	    if (brightness < 1) {
		QColor bgColor = getBackgroundColor();
		int r = bgColor.red();
		int g = bgColor.green();
		int b = bgColor.blue();

		QColor color = brush.color();
		r = r + (int)((color.red() - r) * brightness);
		g = g + (int)((color.green() - g) * brightness);
		b = b + (int)((color.blue() - b) * brightness);

		correctedBrush = QBrush(QColor(r, g, b), brush.style());
	    }

	    painter->setBrush(correctedBrush);
	}
    }
}

void CellWidget::setBrushOrigin(const QPoint& origin) {
    if (painter) {
	painter->setBrushOrigin(brushDx + origin.x(), brushDy + origin.y());
    }
}

