#ifndef IMAGECOLLECTION_H
#define IMAGECOLLECTION_H

#include <QImage>
#include <QList>
#include <QStringList>

class ImageCollection
{
public:
    ImageCollection();
    QImage getImage(int id);
    QList<int> getIds(QStringList tags);
    QStringList getTags();
    void addTag(int id, QString tag);
    void addImage(QImage images, QStringList tags);
    void updateTags(QString, QString);
    void deleteTags( QString tag );
    void editTag(QString tagName);
    void removeTag(QString tagName);
    void removeImage(QImage image);
private:
    void populateDatabase();
};

#endif // IMAGECOLLECTION_H
