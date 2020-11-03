/*!
 *  ======== Warnings ========
 *  Conditional warnings manager
 *  @nodoc
 *  This module declares all warnings generated by the packages in the xdc.*
 *  name space. Each warning is identified with an ID, a positive integer
 *  number. A warning message allows conversion specifiers that can be replaced 
 *  with actual strings at the time a warning message is generated.
 *  The purpose of IDs is to let users find warning documentation in this module
 *  and also to allow warnings to be disabled based on their IDs.
 */
metaonly module Warnings {
    typedef String StringArray[];

    /*!
     *  ======== Warning ========
     *  Warning descriptor
     *
     *  Warnings are defined as instances of this structure, and also that's
     *  how the warnings are kept internally.
     */
    struct Warning {
        Int id;
        String msg;
    }

    /*!
     *  ======== WarningParams ========
     *  Structure passed to $logWarning
     *
     *  Currently, the only parameter assigned to a warning is an array of
     *  strings to replace conversion specifiers in `Warning`s. In the future,
     *  a caller can identify a module where the warning is defined, which will
     *  allow any package to introduce new warnings.
     */
    struct WarningParams {
        Warning warning;
        StringArray args;
    }

    /*!
     *  ======== LOGID ========
     *  Multiple Log events with the same ID
     *
     *  If two Log events share a message, they are automatically assigned
     *  the same ID because IDs also serve as pointers to an array that holds
     *  a compact representation of message strings. To avoid duplication of
     *  strings, if a string already exists in the array, the ID of the 
     *  existing copy is used.
     *
     *  This warning can be useful for package producers who unintentionally
     *  declare two events with the same message, which could be combined
     *  into one common Log event.
     */
    const Warning LOGID = {
        id: 4,
        msg: "Two events with the same id: %s and %s share the msg: '%s'. You must reconfigure the message of one of these events to ensure its id will be unique."
    };

    /*!
     *  ======== disable ========
     *  Internal function that disables warnings
     *
     *  @param(ids)    array of IDs
     */
    Void disable(StringArray ids);

    /*!
     *  ======== getMessage ========
     *  Internal function that creates an actual output message
     *
     *  @param(prms)    WarningParams structure passed by the warning
     *                  generation code
     */
    String getMessage(WarningParams prms);
}
/*
 *  @(#) xdc; 1, 1, 1,0; 5-22-2018 17:31:52; /db/ztree/library/trees/xdc/xdc-E05/src/packages/
 */
