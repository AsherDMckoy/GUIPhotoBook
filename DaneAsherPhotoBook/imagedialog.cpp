/**********************************************
 * Name:Dane Cabb & Asher McKoy               *
 * Class: GUI Programming                     *
 * Due Date: 18th April 2017                  *
 * Program : DataBase/Photobook               *
 * Section: 1                                 *
 * ********************************************/

#include "imagedialog.h"
#include "ui_imagedialog.h"
#include "imagecollection.h"
#include <QMessageBox>
#include <QDebug>

ImageDialog::ImageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageDialog)
{
    ui->setupUi(this);

    currentImage = -1;

    updateTags();
    updateImages();

    /*enables buttons*/

    ui->editTagButton->setEnabled(false);
    ui->delImageButton->setEnabled(false);
    ui->removeTagButton->setEnabled(false);
    ui->previousButton->setEnabled(false);
    ui->nextButton->setEnabled(false);

    /*SIGNALS and SLOTS for button fucntionality*/

    connect(ui->previousButton,SIGNAL(clicked(bool)), this,SLOT(previousClicked()));
    connect(ui->nextButton,SIGNAL(clicked(bool)),this,SLOT(nextClicked()));
    connect(ui->addTagButton,SIGNAL(clicked(bool)),this,SLOT(addTagClicked()));
    connect(ui->addImageButton,SIGNAL(clicked(bool)),this,SLOT(addImageClicked()));
    connect(ui->tagList, SIGNAL(itemSelectionChanged()),this, SLOT(tagsChanged()));
    connect(ui->editTagButton,SIGNAL(clicked()),this,SLOT(editTagButtonClicked()));
    connect(ui->removeTagButton, SIGNAL(clicked()), this, SLOT(removeTagClicked()) );
    connect(ui->delImageButton, SIGNAL(clicked()), this, SLOT(removeImageClicked()) );
}

ImageDialog::~ImageDialog()
{
    delete ui;
}

void ImageDialog::nextClicked() //function nextClicked
{
    currentImage = (currentImage+1) % imageIds.count(); //finds position of next image
    updateCurrentImage(); //updates current image window to next
    qDebug()<<"next image";

}

void ImageDialog::previousClicked() //function previousClicked
{
    currentImage --; //decrements current image position
    if(currentImage == -1)
    {
        currentImage = imageIds.count()-1; //moves to previous image position
    }
    updateCurrentImage(); //update currents image
    qDebug()<<"previous image";
}

void ImageDialog::tagsChanged() //function tagsChanged
{
    updateImages(); //updates Images shown in book
}

void ImageDialog::addImageClicked() //function addImageClicked
{
    qDebug()<<"image added succes";
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),QString(),tr("PNG Images (*.PNG)")); //open image files
    if( !filename.isNull())
    {
        QImage image(filename); //gets image name
        if(image.isNull())
        {
        QMessageBox::warning(this, tr("Image Book"), tr("Failed to open file '%1'").arg(filename)); //if image name is empty addname
        return;
        }
        images.addImage(image, selectedTags()); //add image to book

        updateImages(); //updateImages
    }
}

void ImageDialog::addTagClicked() //fucntion addTagClicked
{
    bool ok;
    qDebug()<<"Tag was added";
    QString tag = QInputDialog::getText(this,tr("Image Book"), tr("Tag:"), QLineEdit::Normal, QString(), &ok); //used for tag text

    if(ok)
    {
        tag = tag.toLower(); //converts tag text to lowercase
        QRegExp re("[a-z]+" );
        if(re.indexIn(tag)== -1){
            QMessageBox::warning(this, tr("Image Book"), tr("This is not a valid tag."), tr("Tags consists of lower cases characters a-z.")); //shows warning for invalid tag name
            return;
        }
        images.addTag(imageIds[currentImage], tag); //adds tag to image
        /*enables buttons remove and edit for use*/
        ui->editTagButton->setEnabled(true);
        ui->removeTagButton->setEnabled(true);
        updateTags(); //updates image tags
    }
}

void ImageDialog::updatedtags() //fucntion updatetags
{
    qDebug()<<"tag updated";
    QStringList selection = selectedTags(); //creates string list
    bool ok;
    QString tag = QInputDialog::getText(
      this, tr("Image Book"), tr("New Tag:"), //gets image tag
      QLineEdit::Normal,QString(), &ok); //runs new q line edit

    if(tag.isEmpty())
    {
        QMessageBox::warning(this,tr("Error"),tr("Empty String Not Allowed! Please Try Again.")); //error if user tries empty string
        return;
    }
    else{
        images.updateTags(tag,selection.join("")); //update tag
        updateTags();
    }
}

QStringList ImageDialog::selectedTags() //fucntion selectedTags
{

    QStringList result; //creates string list
    foreach( QListWidgetItem * item, ui->tagList->selectedItems())
        result <<item->text(); //adds items to list
    return result;
}

void ImageDialog::editTagButtonClicked() //function edittagButtonClicked
{
    qDebug()<<"edit tag";
    int index = ui->tagList->currentRow(); //finds current row index
    QString tagName;

    if ( index < 0 )
    {
        QMessageBox::warning(this,"Edit Tag Error","Please select tag to edit"); //shows warning
        return;
    }
    else
    {
        tagName = ui->tagList->item(index)->text();//loads tag name to string

        images.editTag(tagName); //edit tags
    }
    updateTags(); //updates tag
}

void ImageDialog::updateTags() //function updateTags
{  qDebug()<<"update tags";
    QStringList selection = selectedTags(); //string list

    QStringList tags = images.getTags(); //list for tags
    ui->tagList->clear();
    ui->tagList->addItems(tags);

    /*lods tag names to list*/
    for(int i=0; i<ui->tagList->count();i++){
        if(selection.contains(ui->tagList->item(i)->text()))
            ui->tagList->item(i)->setSelected(true);
    }
}

void ImageDialog::updateImages() //function updateImages
{
    int id;

    qDebug()<<"update image";
    if(currentImage != -1)
     id = imageIds[currentImage];
    else
     id = -1;

    /*gets tags and updates tags of current images*/
    imageIds = images.getIds(selectedTags());
    currentImage = imageIds.indexOf(id);
    if(currentImage == -1 && !imageIds.isEmpty())
        currentImage = 0;

    //sets label of image

    ui->label->setText(QString::number(imageIds.count()));
    if(imageIds.count() > 0 )
    {
        currentImage = (currentImage+1) % imageIds.count();
    }
    updateCurrentImage(); //update current image

}

void ImageDialog::updateCurrentImage() //updateCurrentImage
{
    qDebug()<<"At current image";
    if(currentImage == -1)
    {
        //sets image label
        ui->imageLabel->setPixmap(QPixmap());
        ui->imageLabel->setText(tr("No Image"));

        //enables buttons for use
        ui->addTagButton->setEnabled(false);
        ui->nextButton->setEnabled(false);
        ui->previousButton->setEnabled(false);
    }
    else
    {
        //enables buttons
        ui->imageLabel->setPixmap(QPixmap::fromImage(images.getImage(imageIds[currentImage])));
        ui->addTagButton->setEnabled(true);
        ui->delImageButton->setEnabled(true);
        ui->previousButton->setEnabled(true);
        ui->nextButton->setEnabled(true);

    }

}

void ImageDialog::removeTagClicked() //function removeTagClicked
{   qDebug()<<"removed tag";
    int index = ui->tagList->currentRow(); //current list index

    QString tagName;

    if ( index < 0 )
    {
        QMessageBox::warning(this,"Remove tag message","Please Select tag to delete"); //removes slected tag

        return;
    }
    else
    {
        tagName = ui->tagList->item(index)->text(); //changes list tag texxt
        images.removeTag(tagName); //removes tag

    }
    updateTags(); //update tags
}


void ImageDialog::removeImageClicked() //function removeImageClicked
{
    qDebug()<<"delete function";
    QImage image; //creat image object

   const QPixmap *picture = ui->imageLabel->pixmap(); //create pixmap object

    image = picture->toImage(); //maps pictture to image

    images.removeImage(image); //removes image

    ui->addImageButton->setEnabled(true); //enables add image button



    updateImages(); //upate images
    qDebug()<<"delete function end";


}

