#include "qxtglobalshortcut.h"

#include "qxtglobalshortcut_p.h"

#include <QAbstractEventDispatcher>
#include <QApplication>

#ifndef Q_OS_MAC
int QxtGlobalShortcutPrivate::ref = 0;
#   if QT_VERSION < QT_VERSION_CHECK(5,0,0)
QAbstractEventDispatcher::EventFilter QxtGlobalShortcutPrivate::prevEventFilter = 0;
#   endif
#endif // Q_OS_MAC
QHash<QPair<quint32, quint32>, QxtGlobalShortcut*> QxtGlobalShortcutPrivate::shortcuts;

QxtGlobalShortcutPrivate::QxtGlobalShortcutPrivate(QxtGlobalShortcut *q,QObject* parent)
    : enabled(true)
    , key(Qt::Key(0))
    , mods(Qt::NoModifier)
    , nativeKey(0)
    , nativeMods(0)
    , registered(false)
    , q_ptr(q)
{
#ifndef Q_OS_MAC
    if (ref == 0) {
#   if QT_VERSION < QT_VERSION_CHECK(5,0,0)
        prevEventFilter = QAbstractEventDispatcher::instance()->setEventFilter(eventFilter);
#   else
	qApp->installNativeEventFilter(this);
#endif
    }
    ++ref;
#endif // Q_OS_MAC
	if (parent && parent->isWidgetType())
	{
		QWidget* w = qobject_cast<QWidget*>(parent);
		winId = w->winId();
		//
	}
}

QxtGlobalShortcutPrivate::~QxtGlobalShortcutPrivate()
{
    unsetShortcut();

#ifndef Q_OS_MAC
    --ref;
    if (ref == 0) {
        QAbstractEventDispatcher *ed = QAbstractEventDispatcher::instance();
        if (ed != nullptr) {
#   if QT_VERSION < QT_VERSION_CHECK(5,0,0)
            ed->setEventFilter(prevEventFilter);
#   else
            ed->removeNativeEventFilter(this);
#   endif
        }
    }
#endif // Q_OS_MAC
}

bool QxtGlobalShortcutPrivate::setShortcut(const QKeySequence& shortcut)
{
    unsetShortcut();

    const Qt::KeyboardModifiers allMods =
            Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier;
    const auto xkeyCode = static_cast<uint>( shortcut.isEmpty() ? 0 : shortcut[0] );
    // WORKAROUND: Qt has convert some keys to upper case which
    //             breaks some shortcuts on some keyboard layouts.
    const uint keyCode = QChar::toLower(xkeyCode & ~allMods);

    key = Qt::Key(keyCode);
    mods = Qt::KeyboardModifiers(xkeyCode & allMods);

    nativeKey = nativeKeycode(key);
    nativeMods = nativeModifiers(mods);

    registered = registerShortcut(nativeKey, nativeMods);
    //if (registered)
    shortcuts.insert(qMakePair(nativeKey, nativeMods), q_ptr);

    return registered;
}

bool QxtGlobalShortcutPrivate::unsetShortcut()
{
    if (registered
            && shortcuts.value(qMakePair(nativeKey, nativeMods)) == q_ptr
            && unregisterShortcut(nativeKey, nativeMods))
    {
        shortcuts.remove(qMakePair(nativeKey, nativeMods));
        registered = false;
        return true;
    }

    return false;
}

void QxtGlobalShortcutPrivate::activateShortcut(quint32 nativeKey, quint32 nativeMods)
{
	auto pair = qMakePair(nativeKey, nativeMods);
    QxtGlobalShortcut* shortcut = shortcuts.value(pair);
    if (shortcut)
	{ 
        emit shortcut->activated(shortcut);
	}
}

/*!
    \class QxtGlobalShortcut
    \brief The QxtGlobalShortcut class provides a global shortcut aka "hotkey".

    A global shortcut triggers even if the application is not active. This
    makes it easy to implement applications that react to certain shortcuts
    still if some other application is active or if the application is for
    example minimized to the system tray.

    Example usage:
    \code
    QxtGlobalShortcut* shortcut = new QxtGlobalShortcut(window);
    connect(shortcut, SIGNAL(activated()), window, SLOT(toggleVisibility()));
    shortcut->setShortcut(QKeySequence("Ctrl+Shift+F12"));
    \endcode
 */

/*!
    \fn QxtGlobalShortcut::activated()

    This signal is emitted when the user types the shortcut's key sequence.

    \sa shortcut
 */

/*!
    Constructs a new QxtGlobalShortcut with \a parent.
 */
QxtGlobalShortcut::QxtGlobalShortcut(QObject* parent)
    : QObject(parent)
    , d_ptr(new QxtGlobalShortcutPrivate(this,parent))
{
}

/*!
    Constructs a new QxtGlobalShortcut with \a shortcut and \a parent.
 */
QxtGlobalShortcut::QxtGlobalShortcut(const QKeySequence& shortcut, QObject* parent)
    : QxtGlobalShortcut(parent)
{
    setShortcut(shortcut);
}

/*!
    Destructs the QxtGlobalShortcut.
 */
QxtGlobalShortcut::~QxtGlobalShortcut()
{
    delete d_ptr;
}

/*!
    \property QxtGlobalShortcut::shortcut
    \brief the shortcut key sequence

    Notice that corresponding key press and release events are not
    delivered for registered global shortcuts even if they are disabled.
    Also, comma separated key sequences are not supported.
    Only the first part is used:

    \code
    qxtShortcut->setShortcut(QKeySequence("Ctrl+Alt+A,Ctrl+Alt+B"));
    Q_ASSERT(qxtShortcut->shortcut() == QKeySequence("Ctrl+Alt+A"));
    \endcode

    \sa setShortcut()
 */
QKeySequence QxtGlobalShortcut::shortcut() const
{
    return QKeySequence( static_cast<int>(d_ptr->key | d_ptr->mods) );
}

/*!
    \property QxtGlobalShortcut::shortcut
    \brief sets the shortcut key sequence

    \sa shortcut()
 */
bool QxtGlobalShortcut::setShortcut(const QKeySequence& shortcut)
{
    return d_ptr->setShortcut(shortcut);
}

/*!
    \property QxtGlobalShortcut::enabled
    \brief whether the shortcut is enabled

    A disabled shortcut does not get activated.

    The default value is \c true.

    \sa setEnabled(), setDisabled()
 */
bool QxtGlobalShortcut::isEnabled() const
{
    return d_ptr->enabled;
}

/*!
    \property QxtGlobalShortcut::valid
    \brief whether the shortcut was successfully set up
 */
bool QxtGlobalShortcut::isValid() const
{
    return d_ptr->registered;
}

/*!
    Sets the shortcut \a enabled.

    \sa enabled
 */
void QxtGlobalShortcut::setEnabled(bool enabled)
{
    d_ptr->enabled = enabled;
}

/*!
    Sets the shortcut \a disabled.

    \sa enabled
 */
void QxtGlobalShortcut::setDisabled(bool disabled)
{
    d_ptr->enabled = !disabled;
}
