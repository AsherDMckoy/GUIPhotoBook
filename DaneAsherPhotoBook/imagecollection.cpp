/**********************************************
 * Name:Dane Cabb & Asher McKoy               *
 * Class: GUI Programming                     *
 * Due Date: 18th April 2017                  *
 * Program : DataBase/Photobook               *
 * Section: 1                                 *
 * ********************************************/

#include "imagecollection.h"
#include <QtSql/QSqlDatabase>
#include <QMessageBox>
#include <QFile>
#include <QtSql/QtSql>
#include <QImageWriter>
#include <QImageReader>
#include <QFile>
#include <QInputDialog>
#include <QLineEdit>


ImageCollection::ImageCollection() //function image collection
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); //creating a databse object to connect to SQLite databses
    db.setDatabaseName(":memory:"); //sets database's name
    if(db.open()) //checks if conncetion and open database 
        populateDatabase(); //calls function to fill database
    else
       QMessageBox::warning(0,QObject::tr("Failed"),"Database Error:'Could Not Open'"+ db.lastError().text()); //notifies user if databse error occurs
}

QImage ImageCollection::getImage( int id  ) //funtion get image
{

    QSqlQuery query; //create SQL object
    query.prepare("SELECT data FROM images WHERE id = :id"); //prepares the functionality of database
    query.bindValue(":id",id); //binds the id field in database

    if(!query.exec())
        qFatal("Failed to get Image ID!"); //runs if image could not be obtained
    if(!query.next())
        qFatal("Failed to get Image ID!"); //runs if image could nont be obtained

        QByteArray ba = query.value(0).toByteArray(); //creates a qbyte array from query object
        QBuffer buffer(&ba); //creates new buffer
        buffer.open(QIODevice::ReadOnly); //opens buffer for reading

        QImageReader reader(&buffer, "PNG"); //creates readrr from buffer
        QImage img = reader.read(); //reads the image

    return img; //returns the image
}

QList <int> ImageCollection::getIds(QStringList tags) //getIds function
{
    QSqlQuery query;
    if(tags.count() == 0)
        query.prepare("SELECT images.id FROM images"); //creates SQL query
    else
        query.prepare("SELECT id FROM tags WHERE tag IN('"+
                      tags.join("','")+"') GROUP BY id"); //creates query from joined tags yoou didn't read this sir


    if(!query.exec())
    qFatal("Failed to get IdsQ"); //shows error if id could not be found

    QList<int> results; //creates a list of ints
    while(query.next())
        results <<query.value(0).toInt(); //runs until no more fields in database

    return results; //returns the resultant list
}

QStringList ImageCollection::getTags() //getTags function
{
    QSqlQuery query;
    query.prepare(QString("SELECT DISTINCT tag FROM tags")); //SQL query to select images from tag

    if(!query.exec())
        qFatal("Query Failed to execute!"); //if fails sends error that query failed

    QStringList results; //creates string list of results
    while(query.next())
    {
        results << query.value(0).toString(); //loads list with fields obtained from query
    }
    return results; //returns rsultant list
}

void ImageCollection::addTag( int id, QString tag) //function addTag
{
    QSqlQuery query;
    query.prepare(QString("INSERT INTO tags(id,tag) VALUES (:id,:tag)")); //prepares SQL query to insert strings

    query.bindValue(":id",id); //binds id field
    query.bindValue(":tag",tag); //binds tag field

    if(!query.exec())
        qFatal("Query Failed to execute!"); //if it fails to execute show error
}

void ImageCollection::addImage(QImage image, QStringList tags) //function addImagde
{
    QBuffer buffer; //creates qBuffer
    QImageWriter writer(&buffer, "PNG"); //creates image writer
    writer.write(image); //writes the image

    QSqlQuery query; //query objecct

    int id;
    query.prepare("SELECT COUNT(*) FROM images"); //prepares SQL query
    query.exec(); //execustes query statement
    query.next(); //next query field
    id = query.value(0).toInt()+1; //fomrs id

    /*cretaes new query to retrieve images and executes from bound values*/

    query.prepare("INSERT INTO images (id,data) VALUES (:id,:data)");
    query.bindValue(":id", id);
    query.bindValue(":data",buffer.data());
    query.exec();

    foreach(QString tag,tags)
        addTag(id, tag); //add tag to image
}

void ImageCollection::updateTags(QString newtag, QString oldtag) //function updaTags
{
    QSqlQuery tagName; //creat SQL query object

    if(!tagName.exec("UPDATE tags SET tag='" + newtag + "' WHERE tag='" + oldtag + "'"))
        qFatal("The query fails to execute! Query Error"); //shows error if query fails

    return; //end function
}

void ImageCollection::deleteTags( QString tag ) //function delete tags
{
    QSqlQuery tagName; //create query name

    if(!tagName.exec("DELETE FROM tags WHERE tag='" + tag + "'")) //execute  the query
        qFatal("The query fails to execute! Query Error"); //error occurs when query fails

    return; //returns
}


void ImageCollection::populateDatabase() //function populateDatabase
{
    QSqlQuery query; //creates query object
    if(!query.exec(QString("CREATE TABLE IF NOT EXISTS images(id INTEGER PRIMARY KEY NOT NULL,data BLOB)")))
        QMessageBox::warning(0,QObject::tr("Database Error"),"Failed to create Table images\n" + query.lastError().text());//shows warning if table cannot be created

    if(!query.exec(QString("CREATE TABLE IF NOT EXISTS tags(id INTEGER NOT NULL,tag VARCHAR(30))")))
    QMessageBox::warning(0,QObject::tr("Database Error"),"Failed to create Table tags\n" + query.lastError().text()); //error if tags cannot be created
}

void ImageCollection::editTag(QString tagName) //function editTag
{
    bool OK;

     QSqlQuery command; //SQL query object

    QString tag = QInputDialog::getText(0, "Image Book","Changes: ", QLineEdit::Normal, QString(), &OK); //used to create image tag

    if ( tag.isEmpty() || tag.isNull() || tag == "")
    {
        QMessageBox::warning(0,"Empty Tag", "Tag cannot be Empty!"); //error if user attempts empty tag
        editTag(tagName); //edits tag to user input
    }

    command.prepare("UPDATE tags SET tag = ? WHERE tag = ?"); //prepares query

    command.bindValue(0 , tag);

    command.bindValue(1 , tagName);

    if ( !command.exec() )
    {
        QMessageBox::warning(0,"Tag Error","Error when updating the tag. "); //shows error if unsuccessful update
    }
}

void ImageCollection::removeTag(QString tagName) //function removeTag
{
    QSqlQuery query; //creates query object

    query.prepare("DELETE FROM tags WHERE tag = ?"); //prepares query

    query.bindValue(0,tagName); //binds tag

    if ( !query.exec() )
    {
        QMessageBox::warning(0,"Tag Error","Tag could not be removed."); //error if unsuccessful tag removal
    }
}

void ImageCollection::removeImage(QImage image) //function removeImage
{

    QBuffer buffer; //creates buffer

    QImageWriter writer(&buffer,"PNG"); //creates image writer

    writer.write(image); //writes image

    QByteArray data = buffer.data(); //creates byte array

      QSqlQuery query; //query object

     // query.prepare("DELETE FROM images WHERE data = '" + data + "'");
      query.prepare("DELETE FROM images WHERE data = ?"); //prepares query

      query.bindValue(0,data); //bouds data from query

      if (!query.exec() )
      {
          QMessageBox::warning(0," Message ","Image Not Deleted"); //error if mage could not be deleted

         return; //returns
      }
}

