ReliableROSData ::= SEQUENCE {
    rosOperation INTEGER,
			-- the operation being applied
    data ANY,
			-- the Operation Argument
    oldVersion ANY
			-- data version to which operation should be applied
			-- version syntax will be defined by the
			-- Reliable ROS user
    newVersion ANY
			-- version number which results from operation
    }

