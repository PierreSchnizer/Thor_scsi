"""Create a few engineering tolerance commands

Just a demonstration of how it can be done

Uses a lattice to find out the index for each element.
Furthermore uses the lattice to work on the index
"""
from numpy.random import Generator, PCG64
from thor_scsi.factory import accelerator_from_config
from thor_scsi.utils import engineering
import os.path
import itertools

# read in the lattice file to be able to find the elements
filename = os.path.join("lattices", "tme_rb.lat")
acc = accelerator_from_config(filename)

# Select the elements: element info only stores the index
# currently no extra inforamtion
element_infos = [
    engineering.ElementInfo(element_index=acc.find("QF", 0).index),
    engineering.ElementInfo(element_index=acc.find("QD", 0).index),
    engineering.ElementInfo(element_index=acc.find("QF", 1).index),
    engineering.ElementInfo(element_index=acc.find("QD", 1).index),
]
print("Used elements\n", element_infos)

# Define which properties you want to change ... it is reading a property
# and setting it back.
# The change is handled in the engineering command
t_prop = engineering.Property(
    element_info=None, set_method_name="setMultipoles", get_method_name="getMultipoles"
)
t_prop_main = engineering.Property(
    element_info=None,
    set_method_name="setMainMultipoleStrength",
    get_method_name="getMainMultipoleStrength",
)

# Define the used distributions. distribution_name is a method of
# the random number generator. See further below
eng_cmds = [
    engineering.ScaleEngineeringDistributionCommand(
        t_property=t_prop_main,
        loc=0,
        size=1e-3,
        distribution_name="normal",
        vector_length=1,
    ),
    engineering.ScaleEngineeringDistributionCommand(
        t_property=t_prop, loc=0, size=1e-3, distribution_name="normal", vector_length=6
    ),
    engineering.AddEngineeringDistributionCommand(
        t_property=t_prop, loc=0, size=1e-3, distribution_name="normal", vector_length=6
    ),
]
print(eng_cmds)

# Now combined the set of commands with the different elements
# todo: review if these should be implemented using cycler or
# using cycler
dist_cmds = list(
    itertools.chain(
        *[engineering.create_distribution_commands(element_infos, ec) for ec in eng_cmds]
    )
)
print("Distribution commands")
for dc in dist_cmds:
    print("\t", dc)
del dc

# All commands above just described which random distribution to
# apply to the different elements. Now these are evaluated using
# a random number generator to create deterministic commands.
# These can be inspected (or even filtered ....)
rng = Generator(PCG64(seed=355113))
cmds = engineering.create_commands_for_randomised_state(dist_cmds, rng=rng)
print("EngineeringCommands for a specific set ")
for c in cmds:
    print("\t", c)
del c

# The deterministic commands are now applied to the different
# elements of the accelerator
# Accelerator does not yet support copy: thus applying factors
# would create a new accelerator. This particular functionality
# is under investigation
acc = engineering.apply_factors(acc, cmds, copy=False)

# And now lets see how it works
for elem in acc:
    print(repr(elem))
