#ifndef QXTGLOBALSHORTCUT_H


#define QXTGLOBALSHORTCUT_H

//#include "private/qxtglobalshortcut_export.h"

#include <QObject>
#include <QKeySequence>

class QxtGlobalShortcutPrivate;

class QxtGlobalShortcut : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool valid READ isValid)
    Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut)

public:
    explicit QxtGlobalShortcut(QObject* parent = nullptr);
    explicit QxtGlobalShortcut(const QKeySequence& shortcut, QObject* parent = nullptr);
    ~QxtGlobalShortcut();

    QKeySequence shortcut() const;
    bool setShortcut(const QKeySequence& shortcut);

    bool isEnabled() const;

    bool isValid() const;

public Q_SLOTS:
    void setEnabled(bool enabled = true);
    void setDisabled(bool disabled = true);

Q_SIGNALS:
    void activated(QxtGlobalShortcut *self);

private:
    friend class QxtGlobalShortcutPrivate;
    QxtGlobalShortcutPrivate *d_ptr;
};

#endif // QXTGLOBALSHORTCUT_H
